#include "SteinerTree.h"
#include <algorithm>
#include <limits>

std::vector<int> SteinerTreeSolver::FindKNearestNeighbors(
	const std::vector<geometry::Point>& nodes,
	const geometry::Point& point,
	int k)
{
	std::vector<std::pair<double, int> > dists;
	dists.reserve(nodes.size());

	for (size_t idx = 0; idx < nodes.size(); ++idx)
	{
		double d = geometry::Distance(point, nodes[idx]);
		dists.emplace_back(d, static_cast<int>(idx));
	}

	std::sort(dists.begin(), dists.end());

	std::vector<int> result;
	result.reserve(k);
	for (int i = 0; i < std::min(k, static_cast<int>(dists.size())); ++i)
	{
		result.push_back(dists[i].second);
	}

	return result;
}

std::vector<std::tuple<int, int, int> > SteinerTreeSolver::GenerateCloseTriplets(
	const std::vector<geometry::Point>& nodes,
	int maxTriplets)
{
	std::vector<std::tuple<int, int, int> > triplets;

	if (nodes.size() < 3)
	{
		return triplets;
	}

	// For each point, find nearest neighbors and generate triplets
	for (size_t i = 0; i < nodes.size() && triplets.size() < static_cast<size_t>(maxTriplets); ++i)
	{
		// Find 5 nearest neighbors to point i
		auto neighbors = FindKNearestNeighbors(nodes, nodes[i], 5);

		// Generate triplets from nearest neighbors
		for (size_t a = 0; a < neighbors.size() && triplets.size() < static_cast<size_t>(maxTriplets); ++a)
		{
			for (size_t b = a + 1; b < neighbors.size() && triplets.size() < static_cast<size_t>(maxTriplets); ++b)
			{
				if (static_cast<int>(i) != neighbors[a] && static_cast<int>(i) != neighbors[b])
				{
					triplets.emplace_back(i, neighbors[a], neighbors[b]);
				}
			}
		}
	}

	return triplets;
}

std::vector<std::tuple<int, int, int> > SteinerTreeSolver::GenerateMstEdgeTriplets(
	const Graph& mstGraph,
	int maxTriplets)
{
	std::vector<std::tuple<int, int, int> > triplets;

	if (mstGraph.nodes.size() < 3)
	{
		return triplets;
	}

	// Build adjacency list for MST
	std::vector<std::vector<int> > adjacency(mstGraph.nodes.size());
	for (const auto& edge : mstGraph.edges)
	{
		adjacency[edge.u].push_back(edge.v);
		adjacency[edge.v].push_back(edge.u);
	}

	// For each node, take pairs of neighbors from MST
	for (size_t i = 0; i < mstGraph.nodes.size() && triplets.size() < static_cast<size_t>(maxTriplets); ++i)
	{
		const auto& neighbors = adjacency[i];
		for (size_t a = 0; a < neighbors.size() && triplets.size() < static_cast<size_t>(maxTriplets); ++a)
		{
			for (size_t b = a + 1; b < neighbors.size() && triplets.size() < static_cast<size_t>(maxTriplets); ++b)
			{
				triplets.emplace_back(i, neighbors[a], neighbors[b]);
			}
		}
	}

	return triplets;
}

SteinerTreeSolver::Result SteinerTreeSolver::Compute(const Graph& terminals)
{
	Result res;
	res.graph = MstBoruvka::Compute(terminals);
	res.length = res.graph.TotalWeight();
	res.steinerPointsCount = 0;

	const int maxIterations = 50;
	const int candidatesPerIter = 30;

	std::mt19937 rng(42);

	for (int iter = 0; iter < maxIterations; ++iter)
	{
		bool improved = false;
		geometry::Point bestCandidate;
		std::vector<int> bestNeighbors;
		double maxReduction = 0.0;

		// Strategy 1: Generate triplets based on point proximity
		auto closeTriplets = GenerateCloseTriplets(res.graph.nodes, candidatesPerIter / 2);

		// Strategy 2: Generate triplets based on MST structure
		auto mstTriplets = GenerateMstEdgeTriplets(res.graph, candidatesPerIter / 2);

		// Combine candidates
		std::vector<std::tuple<int, int, int> > allTriplets;
		allTriplets.insert(allTriplets.end(), closeTriplets.begin(), closeTriplets.end());
		allTriplets.insert(allTriplets.end(), mstTriplets.begin(), mstTriplets.end());

		// Add random triplets for diversity
		int randomTripletsCount = candidatesPerIter / 4;
		for (int c = 0; c < randomTripletsCount; ++c)
		{
			int n = static_cast<int>(res.graph.nodes.size());
			if (n < 3)
				break;

			int i = rng() % n;
			int j = rng() % n;
			int k = rng() % n;
			while (j == i)
				j = rng() % n;
			while (k == i || k == j)
				k = rng() % n;

			allTriplets.emplace_back(i, j, k);
		}

		// Evaluate each candidate
		for (const auto& [i, j, k] : allTriplets)
		{
			geometry::Point candidate = geometry::GetFermatPoint(
				res.graph.nodes[i], res.graph.nodes[j], res.graph.nodes[k]);

			// Find 3 nearest nodes to candidate
			std::vector<int> neighbors = FindKNearestNeighbors(res.graph.nodes, candidate, 3);

			// Calculate new connection cost
			double newCost = 0.0;
			for (int nb : neighbors)
			{
				newCost += geometry::Distance(candidate, res.graph.nodes[nb]);
			}

			// MST of 3 points is sum of two shortest sides
			std::vector<double> sides = {
				geometry::Distance(res.graph.nodes[i], res.graph.nodes[j]),
				geometry::Distance(res.graph.nodes[j], res.graph.nodes[k]),
				geometry::Distance(res.graph.nodes[k], res.graph.nodes[i]) };
			std::sort(sides.begin(), sides.end());
			double triMst = sides[0] + sides[1];

			double reduction = triMst - newCost;

			if (reduction > maxReduction)
			{
				maxReduction = reduction;
				bestCandidate = candidate;
				bestNeighbors = neighbors;
			}
		}

		if (maxReduction > 1e-6)
		{
			// Add Steiner point
			res.graph.nodes.push_back(bestCandidate);
			Graph tempGraph;
			tempGraph.nodes = res.graph.nodes;
			Graph newMst = MstBoruvka::Compute(tempGraph);

			if (newMst.TotalWeight() < res.length - 1e-6)
			{
				res.graph = newMst;
				res.length = newMst.TotalWeight();
				res.steinerPointsCount++;
				improved = true;
			}
			else
			{
				// Revert: remove added point
				res.graph.nodes.pop_back();
			}
		}

		if (!improved)
			break;
	}

	return res;
}