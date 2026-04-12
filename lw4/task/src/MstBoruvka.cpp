#include "MstBoruvka.h"

Graph MstBoruvka::Compute(const Graph& inputGraph)
{
	const int n = static_cast<int>(inputGraph.nodes.size());
	if (n == 0)
	{
		return {};
	}

	Graph mst;
	mst.nodes = inputGraph.nodes;

	// Initialize components
	std::vector<int> parent(n);
	std::iota(parent.begin(), parent.end(), 0);
	auto find = [&](int i) {
		while (parent[i] != i)
		{
			parent[i] = parent[parent[i]]; // Path compression
			i = parent[i];
		}
		return i;
	};

	auto unite = [&](const int i, const int j) {
		const int rootI = find(i);
		const int rootJ = find(j);
		if (rootI != rootJ)
		{
			parent[rootI] = rootJ;
			return true;
		}
		return false;
	};

	// Precompute all possible edges for complete graph
	std::vector<Edge> allEdges;
	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			double w = geometry::Distance(inputGraph.nodes[i], inputGraph.nodes[j]);
			allEdges.emplace_back(i, j, w);
		}
	}

	int components = n;
	while (components > 1)
	{
		// For each component, find the cheapest edge connecting it to another
		std::vector<int> closestEdgeIdx(n, -1);

		for (size_t k = 0; k < allEdges.size(); ++k)
		{
			const auto& e = allEdges[k];
			int setU = find(e.u);
			int setV = find(e.v);

			if (setU == setV)
			{
				continue;
			}

			if (closestEdgeIdx[setU] == -1 ||
				allEdges[closestEdgeIdx[setU]].weight > e.weight)
			{
				closestEdgeIdx[setU] = static_cast<int>(k);
			}
			if (closestEdgeIdx[setV] == -1 ||
				allEdges[closestEdgeIdx[setV]].weight > e.weight)
			{
				closestEdgeIdx[setV] = static_cast<int>(k);
			}
		}

		bool addedAny = false;
		for (int i = 0; i < n; ++i)
		{
			if (closestEdgeIdx[i] != -1)
			{
				const auto& e = allEdges[closestEdgeIdx[i]];
				if (unite(e.u, e.v))
				{
					mst.AddEdge(e.u, e.v);
					--components;
					addedAny = true;
				}
			}
		}

		if (!addedAny)
		{
			break; // Should not happen in connected graph
		}
	}

	return mst;
}