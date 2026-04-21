#include "SteinerTree.h"

#include "BoruvkaMST.h"
#include "Geometry.h"
#include "Graph.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace steiner
{
namespace
{

constexpr double kImprovementEpsilon = 1e-10;
constexpr double kPositionEpsilon = 1e-13;
constexpr double kMergeEpsilon = 1e-8;
constexpr int kMaxWeiszfeldIterations = 2000;
constexpr int kMaxRefineIterations = 200;
constexpr int kMaxInsertionRounds = 50;

std::vector<std::vector<std::size_t> > BuildAdjacency(
	std::size_t vertexCount, const std::vector<Edge>& edges)
{
	std::vector<std::vector<std::size_t> > adjacency(vertexCount);
	for (const auto& e : edges)
	{
		adjacency[e.from].push_back(e.to);
		adjacency[e.to].push_back(e.from);
	}
	return adjacency;
}

void RecomputeEdgeWeights(const std::vector<Point>& vertices, std::vector<Edge>& edges) noexcept
{
	for (auto& e : edges)
	{
		e.weight = Distance(vertices[e.from], vertices[e.to]);
	}
}

// Collects Fermat-point candidates. Two families are generated:
//   "Local" triples (a, v, b) where both (a-v) and (v-b) are edges of the
//      current tree. Used only for ordering.
//   "Global" triples - every unordered triple of current vertices. These
//      let the search jump out of local minima that local tree triples cannot
//      reach (e.g. the S-S chain topology of a Steiner tree on a convex hull).
// Callers are expected to test each candidate by inserting it and running a
// full topology/position refinement.
std::vector<Point> CollectFermatCandidates(const std::vector<Point>& vertices, const std::vector<Edge>& treeEdges)
{
	const auto adjacency = BuildAdjacency(vertices.size(), treeEdges);
	std::vector<Point> candidates;

	auto push = [&candidates](const Point& p) {
		for (const auto& existing : candidates)
		{
			if (Distance(existing, p) < kMergeEpsilon)
			{
				return;
			}
		}
		candidates.push_back(p);
	};

	// Ordered list of tree-local triples, best first (by local length gain).
	struct ScoredTriple
	{
		double gain = 0.0;
		Point fermat{};
	};
	std::vector<ScoredTriple> localTriples;
	for (std::size_t v = 0; v < vertices.size(); ++v)
	{
		const auto& neighbors = adjacency[v];
		for (std::size_t i = 0; i < neighbors.size(); ++i)
		{
			for (std::size_t j = i + 1; j < neighbors.size(); ++j)
			{
				const std::size_t a = neighbors[i];
				const std::size_t b = neighbors[j];
				const Point fermat = ComputeFermatPoint(
					vertices[a], vertices[v], vertices[b]);
				const double oldLength =
					Distance(vertices[v], vertices[a])
					+ Distance(vertices[v], vertices[b]);
				const double newLength =
					Distance(fermat, vertices[a])
					+ Distance(fermat, vertices[v])
					+ Distance(fermat, vertices[b]);
				localTriples.push_back({ oldLength - newLength, fermat });
			}
		}
	}
	std::ranges::sort(localTriples,
		[](const ScoredTriple& lhs, const ScoredTriple& rhs) {
			return lhs.gain > rhs.gain;
		});
	for (const auto& [gain, fermat] : localTriples)
	{
		push(fermat);
	}

	// Global triples - every unordered {a, b, c} of current vertices.
	const std::size_t n = vertices.size();
	for (std::size_t a = 0; a < n; ++a)
	{
		for (std::size_t b = a + 1; b < n; ++b)
		{
			for (std::size_t c = b + 1; c < n; ++c)
			{
				const Point fermat = ComputeFermatPoint(
					vertices[a], vertices[b], vertices[c]);
				push(fermat);
			}
		}
	}
	return candidates;
}

// Runs a Weiszfeld-style update on every Steiner point, i.e. moves each one
// towards the weighted (1/distance) average of its current tree neighbors.
// Edge weights are kept in sync. Returns the largest single-point shift.
double WeiszfeldStep(std::vector<Point>& vertices, std::vector<Edge>& edges,
	const std::size_t terminalCount)
{
	const auto adjacency = BuildAdjacency(vertices.size(), edges);
	double maxShift = 0.0;
	for (std::size_t s = terminalCount; s < vertices.size(); ++s)
	{
		const auto& neighbors = adjacency[s];
		if (neighbors.empty())
		{
			continue;
		}
		double sumWX = 0.0;
		double sumWY = 0.0;
		double sumW = 0.0;
		bool collapsed = false;
		Point collapseTarget{};
		for (const std::size_t n : neighbors)
		{
			const double d = Distance(vertices[s], vertices[n]);
			if (d < kPositionEpsilon)
			{
				collapsed = true;
				collapseTarget = vertices[n];
				break;
			}
			const double w = 1.0 / d;
			sumWX += w * vertices[n].x;
			sumWY += w * vertices[n].y;
			sumW += w;
		}
		double newX;
		double newY;
		if (collapsed)
		{
			newX = collapseTarget.x;
			newY = collapseTarget.y;
		}
		else if (sumW < kPositionEpsilon)
		{
			continue;
		}
		else
		{
			newX = sumWX / sumW;
			newY = sumWY / sumW;
		}
		const double dx = newX - vertices[s].x;
		const double dy = newY - vertices[s].y;
		const double shift = std::sqrt(dx * dx + dy * dy);
		vertices[s].x = newX;
		vertices[s].y = newY;
		if (shift > maxShift)
		{
			maxShift = shift;
		}
	}
	RecomputeEdgeWeights(vertices, edges);
	return maxShift;
}

// Iterates Weiszfeld until every Steiner point is stationary within epsilon
void OptimizeSteinerPositions(
	std::vector<Point>& vertices,
	std::vector<Edge>& edges,
	const std::size_t terminalCount)
{
	for (int i = 0; i < kMaxWeiszfeldIterations; ++i)
	{
		const double shift = WeiszfeldStep(vertices, edges, terminalCount);
		if (shift < kPositionEpsilon)
		{
			break;
		}
	}
}

// Alternates MST-rebuild with Weiszfeld until both the topology and the
// Steiner point positions stabilise (or the configured iteration cap is hit).
// Returns the resulting total length.
double RefineTopologyAndPositions(std::vector<Point>& vertices, std::vector<Edge>& edges,
	std::size_t terminalCount)
{
	constexpr BoruvkaMST mstBuilder;
	double previousLength = TotalWeight(edges);
	for (int i = 0; i < kMaxRefineIterations; ++i)
	{
		OptimizeSteinerPositions(vertices, edges, terminalCount);

		Graph graph = Graph::BuildComplete(vertices);
		std::vector<Edge> newEdges = mstBuilder.Build(graph);
		const double newLength = TotalWeight(newEdges);

		edges = std::move(newEdges);
		if (std::abs(previousLength - newLength) < kImprovementEpsilon)
		{
			break;
		}
		previousLength = newLength;
	}
	OptimizeSteinerPositions(vertices, edges, terminalCount);
	return TotalWeight(edges);
}

// Removes Steiner points whose degree in the final tree is below 3 (they do
// not help a Steiner tree) and any Steiner point that coincides with another
// vertex. Re-indexes the edge list in place.
void CleanupSteinerPoints(std::vector<Point>& vertices, std::vector<Edge>& edges,
	std::size_t terminalCount)
{
	auto removeEdgesTouching = [&edges](std::size_t v) {
		std::erase_if(edges,
			[v](const Edge& e) { return e.from == v || e.to == v; });
	};

	bool changed = true;
	while (changed)
	{
		changed = false;

		// Merge any Steiner point that has drifted onto a terminal or another
		// Steiner point with a lower index.
		for (std::size_t s = terminalCount; s < vertices.size() && !changed; ++s)
		{
			for (std::size_t other = 0; other < s; ++other)
			{
				if (Distance(vertices[s], vertices[other]) < kMergeEpsilon)
				{
					for (auto& e : edges)
					{
						if (e.from == s)
							e.from = other;
						if (e.to == s)
							e.to = other;
					}
					// Drop self-loops and duplicates created by the merge.
					std::erase_if(edges,
						[](const Edge& e) { return e.from == e.to; });
					changed = true;
					break;
				}
			}
		}
		if (changed)
			continue;

		const auto adjacency = BuildAdjacency(vertices.size(), edges);
		for (std::size_t s = terminalCount; s < vertices.size(); ++s)
		{
			const auto& neighbors = adjacency[s];
			if (neighbors.empty())
			{
				changed = true;
				break;
			}
			if (neighbors.size() == 1)
			{
				removeEdgesTouching(s);
				changed = true;
				break;
			}
			if (neighbors.size() == 2)
			{
				const std::size_t a = neighbors[0];
				const std::size_t b = neighbors[1];
				removeEdgesTouching(s);
				Edge bridge{};
				bridge.from = a;
				bridge.to = b;
				bridge.weight = Distance(vertices[a], vertices[b]);
				edges.push_back(bridge);
				changed = true;
				break;
			}
		}
	}

	// Compact the vertex array: keep all terminals, plus Steiner points that
	// still appear in some edge. Rename surviving Steiner points sequentially.
	std::vector<bool> used(vertices.size(), false);
	for (std::size_t i = 0; i < terminalCount; ++i)
	{
		used[i] = true;
	}
	for (const auto& e : edges)
	{
		used[e.from] = true;
		used[e.to] = true;
	}

	std::vector<std::size_t> remap(vertices.size(), 0);
	std::vector<Point> compacted;
	compacted.reserve(vertices.size());
	int steinerIdx = 0;
	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		if (!used[i])
			continue;
		remap[i] = compacted.size();
		Point p = vertices[i];
		if (i >= terminalCount)
		{
			p.name = "S" + std::to_string(++steinerIdx);
		}
		compacted.push_back(std::move(p));
	}
	for (auto& e : edges)
	{
		e.from = remap[e.from];
		e.to = remap[e.to];
	}
	vertices = std::move(compacted);
	RecomputeEdgeWeights(vertices, edges);
}

} // namespace

SteinerTreeResult SteinerTreeBuilder::Build(const std::vector<Point>& terminals)
{
	SteinerTreeResult result{};
	result.vertices = terminals;
	if (terminals.size() < 2)
	{
		return result;
	}

	const BoruvkaMST mstBuilder;
	Graph graph = Graph::BuildComplete(result.vertices);
	result.edges = mstBuilder.Build(graph);
	result.totalLength = TotalWeight(result.edges);

	const std::size_t terminalCount = terminals.size();

	// Insert Fermat points greedily
	auto tryInsertBatch = [&](
		const std::vector<Point>& extraPoints,
		std::vector<Point>& outVertices,
		std::vector<Edge>& outEdges,
		double& outLength) -> bool {
		std::vector<Point> trialVertices = result.vertices;
		const std::size_t baseSize = trialVertices.size();
		for (const auto& extraPoint : extraPoints)
		{
			bool dup = false;
			for (const auto& existing : trialVertices)
			{
				if (Distance(existing, extraPoint) < kMergeEpsilon)
				{
					dup = true;
					break;
				}
			}
			if (dup)
			{
				continue;
			}
			Point sp{};
			sp.name = "S" + std::to_string(trialVertices.size() - terminalCount + 1);
			sp.x = extraPoint.x;
			sp.y = extraPoint.y;
			trialVertices.push_back(std::move(sp));
		}
		if (trialVertices.size() == baseSize)
		{
			return false;
		}

		const Graph trialGraph = Graph::BuildComplete(trialVertices);
		std::vector<Edge> trialEdges = mstBuilder.Build(trialGraph);
		const double refinedLength = RefineTopologyAndPositions(trialVertices, trialEdges, terminalCount);
		if (refinedLength + kImprovementEpsilon < outLength)
		{
			outVertices = std::move(trialVertices);
			outEdges = std::move(trialEdges);
			outLength = refinedLength;
			return true;
		}
		return false;
	};

	// greedy
	for (int round = 0; round < kMaxInsertionRounds; ++round)
	{
		// collect candidates
		const std::vector<Point> candidates = CollectFermatCandidates(result.vertices, result.edges);
		std::vector<Point> bestVertices;
		std::vector<Edge> bestEdges;
		double bestLength = result.totalLength;

		// try to insert single-terminal
		for (const auto& fermat : candidates)
		{
			tryInsertBatch({ fermat }, bestVertices, bestEdges, bestLength);
		}

		// try to insert k-terminals starts with k >= 2
		const std::size_t maxBatch = std::min<std::size_t>(candidates.size(), terminalCount);
		for (std::size_t k = 2; k <= maxBatch; ++k)
		{
			std::vector<Point> prefix(candidates.begin(), candidates.begin() + k);
			tryInsertBatch(prefix, bestVertices, bestEdges, bestLength);
		}

		// found the best version of insertion
		if (bestVertices.empty())
		{
			break;
		}
		result.vertices = std::move(bestVertices);
		result.edges = std::move(bestEdges);
		result.totalLength = bestLength;
	}

	CleanupSteinerPoints(result.vertices, result.edges, terminalCount);
	result.totalLength = TotalWeight(result.edges);
	return result;
}

} // namespace steiner