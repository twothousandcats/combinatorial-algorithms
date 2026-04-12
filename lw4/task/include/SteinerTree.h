#pragma once
#include "Graph.h"
#include "MstBoruvka.h"
#include <random>
#include <iostream>

class SteinerTreeSolver
{
public:
	struct Result
	{
		Graph graph;
		double length{};
		int steinerPointsCount{};
	};

	// Simple iterative improvement:
	// 1. Start with MST.
	// 2. Randomly sample potential Steiner points (or use centroids of triangles).
	// 3. If adding a point and reconnecting its 3 nearest neighbors reduces total length, keep it.
	// 4. Repeat.
	static Result Compute(const Graph& terminals)
	{
		Result res;
		res.graph = MstBoruvka::Compute(terminals);
		res.length = res.graph.TotalWeight();
		res.steinerPointsCount = 0;

		const int maxIterations = 50;
		const int candidatesPerIter = 20;

		std::mt19937 rng(42);

		for (int iter = 0; iter < maxIterations; ++iter)
		{
			bool improved = false;
			geometry::Point bestCandidate;
			std::vector<int> bestNeighbors;
			double maxReduction = 0.0;

			// Generate candidates: Centroids of random triplets of existing nodes
			for (int c = 0; c < candidatesPerIter; ++c)
			{
				int n = static_cast<int>(res.graph.nodes.size());
				int i = rng() % n;
				int j = rng() % n;
				int k = rng() % n;
				while (j == i)
					j = rng() % n;
				while (k == i || k == j)
					k = rng() % n;

				geometry::Point candidate = geometry::GetTorricelliPoint(
					res.graph.nodes[i], res.graph.nodes[j], res.graph.nodes[k]);

				// Find 3 closest nodes in current graph to candidate
				std::vector<std::pair<double, int> > dists;
				for (int idx = 0; idx < n; ++idx)
				{
					double d = geometry::Distance(candidate, res.graph.nodes[idx]);
					dists.emplace_back(d, idx);
				}
				std::sort(dists.begin(), dists.end());

				// Take top 3
				std::vector<int> neighbors = { dists[0].second, dists[1].second, dists[2].second };

				// Calculate cost change
				double newCost = 0.0;
				for (int nb : neighbors)
				{
					newCost += geometry::Distance(candidate, res.graph.nodes[nb]);
				}

				// Estimate removed cost: This is hard without full topology update.
				// Approximation: Assume we replace the paths between these 3 in the MST.
				// This is a rough heuristic.

				// For strict correctness in a short code, we might skip complex removal logic
				// and just accept if newCost < (sum of pairwise distances / 2)? No.

				// Let's rely on the fact that for a triangle, Steiner cost < MST cost of triangle.
				double mstTriCost = geometry::Distance(res.graph.nodes[i], res.graph.nodes[j]) +
					geometry::Distance(res.graph.nodes[j], res.graph.nodes[k]) +
					geometry::Distance(res.graph.nodes[k], res.graph.nodes[i]);
				// Actually, MST of 3 points is sum of 2 shortest sides.
				std::vector<double> sides = {
					geometry::Distance(res.graph.nodes[i], res.graph.nodes[j]),
					geometry::Distance(res.graph.nodes[j], res.graph.nodes[k]),
					geometry::Distance(res.graph.nodes[k], res.graph.nodes[i])
				};
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
				int newNodeIdx = static_cast<int>(res.graph.nodes.size());
				res.graph.nodes.push_back(bestCandidate);

				// Connect to neighbors
				for (int nb : bestNeighbors)
				{
					res.graph.AddEdge(newNodeIdx, nb);
				}

				// Note: We are NOT removing old edges in this simple heuristic
				// because finding which specific edges to remove from the global MST
				// without creating cycles or disconnecting components requires a full MST recalculation
				// or dynamic tree structure.
				// To keep it valid, we should Recalculate MST on the new set of nodes (Terminals + Steiner).

				// Correct Approach for Heuristic:
				// 1. Add Candidate to Node List.
				// 2. Recompute MST on (Original Terminals + All Added Steiner Points).
				// 3. Check if total weight decreased.

				// Let's do this:
				Graph tempGraph;
				tempGraph.nodes = res.graph.nodes; // Includes new point
				// We need to preserve previous Steiner points too.

				// Recompute MST on all current nodes
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
					// Revert: remove the added node from list (it wasn't added to res.graph.nodes permanently yet?
					// Yes it was pushed. Pop it.)
					res.graph.nodes.pop_back();
				}
			}

			if (!improved)
				break;
		}

		return res;
	}
};