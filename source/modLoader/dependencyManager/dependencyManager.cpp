#include "dependencyManager.h"
#include <vector>
#include <queue>
#include <sstream>

template<typename T>
inline void logVector(std::vector<T> vec) {
    for (T element : vec) {
        std::cout << element << ',';
    }
    std::cout << std::endl;
}


inline std::vector<int> topologicalSort(std::vector<ModManifest>& nodes, std::vector<Edge>& edges) {
    int size = nodes.size();
    std::vector<int> outdegree(size, 0);
    std::queue<int> q;
    std::vector<int> output;

    for (const auto& iEdge : edges)
    {
        outdegree[iEdge.from]++;
    }
    
    for (int i = 0; i < size; i++) {
        if (outdegree[i] == 0) q.push(i);
    }

    // Kahn's algorithm adjusted for edge list. Slower at O(V*E)
    while (!q.empty()) {
        int top = q.front();
        q.pop();
        output.push_back(top);
        for (const Edge& iEdge : edges) {
            if (iEdge.to == top) {
                if (--outdegree[iEdge.from] == 0)
                    q.push(iEdge.from);
            }
        }
    }

    return output;
}

void DependencyManager::registerManifest(ModManifest& manifest) {
    _graph.add(manifest);
}

void DependencyManager::finalize(Logger& logger) {
    auto nodes = _graph.getNodes();

    // DEBUG: if the default initializer has a non-zero size the values might not be false.
    _errNodes.resize(nodes.size(), false);

    // Resolve dependencies and create edges
    for (int i = 0; i < nodes.size(); i++) {
        for (const DependencyManifest& iDependency : nodes[i].dependencies) {
            ModManifest _unused;
            int dependencyIndex = _graph.resolve(iDependency, _unused);
            if (dependencyIndex < 0) {

                if (!iDependency.optional) {
                    logger.log(ERR, "Mod \"%s\" is missing required dependency \"%s\"",
                        nodes[i].name.c_str(), iDependency.name.c_str());
                    _errNodes[i] = true;
                } else {
                    logger.log(VERBOSE, "Mod \"%s\" is missiong optional dependency \"%s\"",
                        nodes[i].name.c_str(), iDependency.name.c_str());
                }

            } else {
                _graph.createEdge(i, dependencyIndex);
            }
        }
    }

    auto edges = _graph.getEdges();
    
    _loadOrder = topologicalSort(nodes, edges);
    
    // cycle detection
    if (_loadOrder.size() < nodes.size()) {
        logger.log(ERR, "There is a dependency cycle in the mod list!");

        // TODO: err out manifests that aren't covered in the loadOrder arr
    }

    // Propagate errors
    if (!_errNodes.empty()) {
        bool recheck;
        do {
            logger.log(DEBUG, "propagating dependency errors");
            recheck = false;
            for (const Edge& iEdge : edges) {
                if (_errNodes[iEdge.to] && !_errNodes[iEdge.from]) {
                    recheck = true;
                    logger.log(ERR, "Mod \"%s\" will not be loaded as there was an error loading its dependency \"%s\"",
                        nodes[iEdge.from].name, nodes[iEdge.to].name);
                    _errNodes[iEdge.from] = true;
                }
            }
        } while (recheck);
    }

    _isFinalized = true;
}

std::string DependencyManager::printGraph() {
    if (!_isFinalized) return "==GRAPH NOT YET FINALIZED==";

    std::vector<ModManifest> nodes = _graph.getNodes();
    std::stringstream stream;

    for (const int i : _loadOrder) {
        stream << nodes[i].name << "\tv" << nodes[i].version.major << "." <<
            nodes[i].version.minor << "." << nodes[i].version.patchNum << "\t" <<
            nodes[i].path << (_errNodes[i] ? "\tERROR" : "") << "\n";
    }

    // clipping off last line ending
    stream.seekp(-1, std::ios::end);
    stream << '\0';

    return stream.str();
}

std::vector<ModManifest> DependencyManager::createLoadOrderVector() {
    std::vector<ModManifest> output;
    std::vector<ModManifest> nodes = _graph.getNodes();

    if (!_isFinalized) return output;

    for (int i : _loadOrder) {
        output.push_back(nodes[i]);
    }

    return output;
}