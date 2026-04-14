#pragma once

#include "Edge.h"
#include "Graph.h"

#include <vector>

namespace steiner
{

// Builds a minimum spanning tree using Boruvka's algorithm.
// The input graph is expected to be connected.
class BoruvkaMST
{
public:
	std::vector<Edge> Build(const Graph& graph) const;
};

} // namespace steiner