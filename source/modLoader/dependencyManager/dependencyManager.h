#pragma once

#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_set>
#include <string>
#include "configParser.h"
#include "logger/logger.h"


/// @brief A graph edge that represent a dependency relationship. The values for `from`
///   and `to` represent the index of the corresponding ModManifest. The dependency is represented by Edge.to.
typedef struct Edge {
    short from;
    short to;
} Edge;

inline std::ostream& operator << (std::ostream& outs, const Edge& e) {
    return outs << e.from << '-' << e.to;
}


/// @brief A wrapper for the DependencyGraph
class DependencyManager {
public:
    void registerManifest(ModManifest& manifest);
    void finalize(Logger& logger);
    std::string printGraph();
    std::vector<ModManifest> createLoadOrderVector();

private:
    bool _isFinalized = false;
    // DependencyGraph _graph;
    std::vector<ModManifest> _nodes;
    std::vector<Edge> _edges;
    std::vector<bool> _errNodes;
    std::vector<int> _loadOrder;
};
