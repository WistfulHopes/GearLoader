#pragma once

#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_set>
#include <string>
#include "configParser.h"
#include "logger/logger.h"

// typedef std::iterator<ModManifest, ModManifest> GraphIter;

/// @brief A graph edge that represent a dependency relationship. The values for `from`
///   and `to` represent the index of the corresponding ModManifest. The dependency is represented by Edge.to.
typedef struct Edge {
    short from;
    short to;
} Edge;

inline std::ostream& operator << (std::ostream& outs, const Edge& e) {
    return outs << e.from << '-' << e.to;
}

class DependencyGraph {
public:
    bool contains(ModManifest& manifest) {
        return std::find(_nodes.begin(), _nodes.end(), manifest) != _nodes.end();
    }

    // TODO: get highest version instead of first found
    /// @brief Resolves the Dependency to a registered ModManifest.
    ///  If none is found returns -1 else returns the index of the returned manifest.
    int resolve(DependencyManifest dependency, ModManifest& retrievedManifest) {
        for (int i = 0; i < _nodes.size(); i++) {
            if (_nodes[i].name.compare(dependency.name) == 0 &&
                    _nodes[i].version >= dependency.minVersion) {
                retrievedManifest = _nodes[i];
                return i;
            }
        }

        return -1;
    }
    void add(ModManifest& manifest) {
        _nodes.push_back(manifest);
    }
    bool createEdge(short manifestIndex, short dependencyIndex) {
        if (manifestIndex < 0 || manifestIndex >= _nodes.size() ||
                dependencyIndex < 0 || dependencyIndex >= _nodes.size()) {
            return false;
        }
        _edges.push_back(Edge{manifestIndex, dependencyIndex});
        return true;
    }
    const std::vector<ModManifest>& getNodes() { return _nodes; }
    const std::vector<Edge>& getEdges() { return _edges; }
private:
    std::vector<ModManifest> _nodes;
    std::vector<Edge> _edges;
};


/// @brief A wrapper for the DependencyGraph
class DependencyManager {
public:
    void registerManifest(ModManifest& manifest);
    void finalize(Logger& logger);
    std::string printGraph();
    std::vector<ModManifest> createLoadOrderVector();
private:
    bool _isFinalized = false;
    DependencyGraph _graph;
    std::vector<bool> _errNodes;
    std::vector<int> _loadOrder;
};
