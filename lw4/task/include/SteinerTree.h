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
	static Result Compute(const Graph& terminals);
};