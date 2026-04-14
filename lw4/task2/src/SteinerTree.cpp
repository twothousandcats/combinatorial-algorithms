#include "SteinerTree.h"

#include "BoruvkaMST.h"
#include "Geometry.h"
#include "Graph.h"

#include <cstddef>
#include <string>
#include <utility>

namespace steiner
{
namespace
{

constexpr double kImprovementEpsilon = 1e-9;

struct InsertionCandidate
{
	bool found = false;
	double gain = 0.0;
	Point fermat{};
};

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

// Scans every triple (a, v, b) where (a-v) and (v-b) are edges of the
// current tree and estimates the local length gain from replacing these
// two edges with three edges incident to the Fermat point of (a, v, b).
InsertionCandidate FindBestInsertion(
	const std::vector<Point>& vertices, const std::vector<Edge>& treeEdges)
{
	const auto adjacency = BuildAdjacency(vertices.size(), treeEdges);
	InsertionCandidate best{};

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
				const double gain = oldLength - newLength;

				if (gain > best.gain + kImprovementEpsilon)
				{
					best.found = true;
					best.gain = gain;
					best.fermat = fermat;
				}
			}
		}
	}
	return best;
}

} // namespace

SteinerTreeResult SteinerTreeBuilder::Build(const std::vector<Point>& terminals) const
{
	SteinerTreeResult result{};
	result.vertices = terminals;
	if (terminals.size() < 2)
	{
		return result;
	}

	BoruvkaMST mstBuilder;
	Graph graph = Graph::BuildComplete(result.vertices);
	result.edges = mstBuilder.Build(graph);
	result.totalLength = TotalWeight(result.edges);

	int steinerCounter = 0;
	while (true)
	{
		const InsertionCandidate candidate =
			FindBestInsertion(result.vertices, result.edges);
		if (!candidate.found)
		{
			break;
		}

		Point steinerPoint{};
		steinerPoint.name = "S" + std::to_string(++steinerCounter);
		steinerPoint.x = candidate.fermat.x;
		steinerPoint.y = candidate.fermat.y;
		result.vertices.push_back(std::move(steinerPoint));

		// Rebuild the MST on the expanded vertex set and accept the new
		// point only if the total length actually decreased.
		graph = Graph::BuildComplete(result.vertices);
		std::vector<Edge> newTree = mstBuilder.Build(graph);
		const double newLength = TotalWeight(newTree);

		if (newLength + kImprovementEpsilon >= result.totalLength)
		{
			result.vertices.pop_back();
			--steinerCounter;
			break;
		}
		result.edges = std::move(newTree);
		result.totalLength = newLength;
	}
	return result;
}

} // namespace steiner