#pragma once
#include "Graph.h"
#include "MstBoruvka.h"
#include <random>
#include <iostream>
#include <vector>
#include <tuple>

class SteinerTreeSolver
{
public:
	struct Result
	{
		Graph graph;
		double length{};
		int steinerPointsCount{};
	};

	// Start with MST
	// Generate candidates from:
	//  - Close triplets of terminals (distance-based selection)
	//  - MST edge neighborhoods (triplets formed by adjacent edges)
	//  - Systematic enumeration of first N^3 triplets for deterministic coverage
	// Use Fermat point for each candidate triplet
	// Keep candidate if it reduces total tree length
	// Repeat until no improvement
	static Result Compute(const Graph& terminals);

private:
	// Find k nearest neighbors to a point
	static std::vector<int> FindKNearestNeighbors(
		const std::vector<geometry::Point>& nodes,
		const geometry::Point& point,
		int k);

	// Generate candidate triplets based on proximity
	static std::vector<std::tuple<int, int, int> > GenerateCloseTriplets(
		const std::vector<geometry::Point>& nodes,
		int maxTriplets);

	// Generate triplets from MST edge neighborhoods
	static std::vector<std::tuple<int, int, int> > GenerateMstEdgeTriplets(
		const Graph& mstGraph,
		int maxTriplets);
};