#pragma once

#include "steiner_lab/geometry.h"
#include <set>
#include <utility>
#include <vector>

namespace steiner_lab
{

struct EuclideanSteinerTree
{
	std::vector<Point2D> steiner_points{};
	double total_length = 0.0;
	std::vector<std::pair<Point2D, Point2D>> segments{};
};

class EuclideanSteinerSolver
{
public:
	[[nodiscard]] EuclideanSteinerTree Solve(const std::vector<Point2D>& terminals) const;

private:
	using NormalizedEdge = std::pair<int, int>;
	using TopologyEdgeSet = std::set<NormalizedEdge>;
	using TopologyCollection = std::set<TopologyEdgeSet>;

	[[nodiscard]] static NormalizedEdge CanonEdge(int endpointA, int endpointB);
	static void InsertTopologyEdge(TopologyEdgeSet& graph, int endpointA, int endpointB);
	static void EnumerateFullTopologiesRec(const std::vector<int>& vertices, int nextSteinerId, TopologyCollection& output);
	[[nodiscard]] static TopologyCollection BuildAllFullTopologies(int terminalCount);
	static void BuildAdjacency(const TopologyEdgeSet& edges, int maxVertexId, std::vector<std::vector<int>>& adjacency);
	[[nodiscard]] static double TotalTreeLength(const TopologyEdgeSet& edges, const std::vector<Point2D>& positionsById);
	[[nodiscard]] static double SmithRelaxationStep(std::vector<Point2D>& positionsById,
		const std::vector<std::vector<int>>& adjacency, int terminalCount);
	[[nodiscard]] static bool RunSmithRelaxation(const TopologyEdgeSet& topology, int terminalCount, int maxVertexId,
		std::vector<Point2D>& positionsById, double tolerance, int maxIterations);
	[[nodiscard]] static int MaxVertexId(const TopologyEdgeSet& edges);
	[[nodiscard]] static Point2D TerminalCentroid(const std::vector<Point2D>& terminals);
	static void PlaceTerminalsInPositionArray(std::vector<Point2D>& positions, int terminalCount,
		const std::vector<Point2D>& terminals);
	static void InitSteinerAtCentroid(std::vector<Point2D>& positions, int terminalCount, int maxVertexId,
		const Point2D& centroid);
	static void AdoptIfShorter(const TopologyEdgeSet& topology, int terminalCount, int maxVertexId,
		const std::vector<Point2D>& positions, double length, double& bestLength, EuclideanSteinerTree& bestTree);
	[[nodiscard]] static EuclideanSteinerTree TrivialEmptyOrSingle();
	[[nodiscard]] static EuclideanSteinerTree TrivialTwoTerminals(const std::vector<Point2D>& terminals);
	[[nodiscard]] static EuclideanSteinerTree TrivialThreeTerminals(const std::vector<Point2D>& terminals);
	static void EvaluateTopology(const TopologyEdgeSet& topology, const std::vector<Point2D>& terminals, int terminalCount,
		double& bestLength, EuclideanSteinerTree& bestTree);

	static void CollectTwoVertexTopology(const std::vector<int>& vertices, TopologyCollection& output);
	static void CollectThreeVertexTopology(const std::vector<int>& vertices, int nextSteinerId, TopologyCollection& output);
	static std::vector<int> MakeReducedVertexListWithSteiner(const std::vector<int>& vertices, std::size_t skipFirstIndex,
		std::size_t skipSecondIndex, int steinerId);
	static bool FindSteinerLeafNeighbor(const TopologyEdgeSet& subgraph, int steinerId, int& outNeighbor);
	static TopologyEdgeSet TransformSubgraphWithCherry(const TopologyEdgeSet& subgraph, int steinerId, int oldNeighbor,
		int endpointA, int endpointB);
	static void AppendCherryExtensionsForPair(const std::vector<int>& vertices, std::size_t skipFirstIndex,
		std::size_t skipSecondIndex, int nextSteinerId, TopologyCollection& output);
};

} // namespace steiner_lab
