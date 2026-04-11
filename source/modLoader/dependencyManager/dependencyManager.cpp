#include "dependencyManager.h"
#include <vector>
#include <queue>
#include <sstream>
#include "common/versionParsing.h"

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
    _nodes.push_back(manifest);
}

/// @brief Resolves the Dependency to a registered ModManifest.
///  If none is found returns -1 else returns the index of the returned manifest.
inline int resolve(DependencyManifest dependency, ModManifest& retrievedManifest, std::vector<ModManifest>& nodes) {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].name.compare(dependency.name) == 0 &&
                CompareVersions(nodes[i].version, dependency.versionOperator, dependency.version)) {
            retrievedManifest = nodes[i];
            return i;
        }
    }

    return -1;
}
void DependencyManager::finalize(Logger& logger) {

    _errNodes.resize(_nodes.size(), false);

    // Independent Error checks
    for (int i = 0; i < _nodes.size(); i++) {
        auto& iNode = _nodes[i];

        if (iNode.ignore) {
            logger.log(VERBOSE, "Mod \"%s\" is marked as ignored.", iNode.name.c_str());
            _errNodes[i] = true;
        }
        static const SemanticVersion curVer = GEARLOADER_VERSION_SEM_VER;
        if (_nodes[i].modLoaderVersion.major > curVer.major) {
            logger.log(ERR, "Mod \"%s\" requries GearLoader v%s, but the current version is v%s",
                iNode.name.c_str(),
                ToString(iNode.modLoaderVersion).c_str(),
                ToString(curVer).c_str()
            );
            _errNodes[i] = true;
        }
    }

    // Resolve dependencies and create edges
    for (int i = 0; i < _nodes.size(); i++) {
        for (const DependencyManifest& iDependency : _nodes[i].dependencies) {
            ModManifest _unused;
            int dependencyIndex = resolve(iDependency, _unused, _nodes);
            if (dependencyIndex < 0) {

                if (!iDependency.optional) {
                    logger.log(ERR, "Mod \"%s\" is missing required dependency \"%s\"",
                        _nodes[i].name.c_str(), iDependency.name.c_str());
                    _errNodes[i] = true;
                } else {
                    logger.log(VERBOSE, "Mod \"%s\" is missiong optional dependency \"%s\"",
                        _nodes[i].name.c_str(), iDependency.name.c_str());
                }

            } else {
                _edges.push_back(Edge{
                    static_cast<short>(i),
                    static_cast<short>(dependencyIndex)});
            }
        }
    }
    
    _loadOrder = topologicalSort(_nodes, _edges);
    
    // cycle detection
    if (_loadOrder.size() < _nodes.size()) {
        logger.log(ERR, "There is a dependency cycle in the mod list!");

        // TODO: This could be more performant
        for(int i = 0; i < _nodes.size(); i++) {
            if (!_errNodes[i] &&
                std::find(_loadOrder.begin(), _loadOrder.end(), i) == _loadOrder.end()) {
                logger.log(ERR, "\tMod \"%s\" is part of a circular dependency", _nodes[i].name.c_str());
                _errNodes[i] = true;
            }
        }
    }

    // Propagate errors
    if (!_errNodes.empty()) {
        bool recheck;
        do {
            recheck = false;
            for (const Edge& iEdge : _edges) {
                if (_errNodes[iEdge.to] && !_errNodes[iEdge.from]) {
                    recheck = true;
                    logger.log(ERR, "Mod \"%s\" will not be loaded as there was an error loading its dependency \"%s\"",
                        _nodes[iEdge.from].name, _nodes[iEdge.to].name);
                    _errNodes[iEdge.from] = true;
                }
            }
        } while (recheck);
    }

    _isFinalized = true;
}

std::string DependencyManager::printGraph() {
    if (!_isFinalized) return "==GRAPH NOT YET FINALIZED==";

    std::stringstream stream;

    for (const int i : _loadOrder) {
        stream << _nodes[i].name << "\tv" << _nodes[i].version.major << "." <<
            _nodes[i].version.minor << "." << _nodes[i].version.patchNum << "\t" <<
            _nodes[i].path << (_errNodes[i] ? "\tERROR" : "") << "\n";
    }

    // clipping off last line ending
    stream.seekp(-1, std::ios::end);
    stream << '\0';

    return stream.str();
}

std::vector<ModManifest> DependencyManager::createLoadOrderVector() {
    std::vector<ModManifest> output;

    if (!_isFinalized) return output;

    for (int i : _loadOrder) {
        if (!_errNodes[i]) output.push_back(_nodes[i]);
    }

    return output;
}
