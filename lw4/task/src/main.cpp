#include <iostream>
#include <vector>
#include <random>
#include "Geometry.h"
#include "Graph.h"
#include "MstBoruvka.h"
#include "SteinerTree.h"
#include "Visualizer.h"

int main()
{
	// 1. Generate Random Terminals
	const int N = 10;
	std::mt19937 rng(12345);
	std::uniform_real_distribution<double> dist(0.0, 100.0);

	Graph terminals;
	for (int i = 0; i < N; ++i)
	{
		terminals.AddNode(geometry::Point(dist(rng), dist(rng)));
	}

	// 2. Compute MST (Boruvka)
	Graph mst = MstBoruvka::Compute(terminals);
	double mstLen = mst.TotalWeight();

	// 3. Compute Steiner Tree (Heuristic)
	auto steinerRes = SteinerTreeSolver::Compute(terminals);
	double steinerLen = steinerRes.length;

	// 4. Output Results
	std::cout << "Terminals: " << N << std::endl;
	std::cout << "MST Length (Boruvka): " << mstLen << std::endl;
	std::cout << "Steiner Length (Heuristic): " << steinerLen << std::endl;
	std::cout << "Improvement: " << ((mstLen - steinerLen) / mstLen * 100.0) << "%" << std::endl;
	std::cout << "Steiner Points Added: " << steinerRes.steinerPointsCount << std::endl;

	// 5. Visualize
	Visualizer::SaveToHtml(mst, steinerRes.graph, "output.html");
	std::cout << "Visualization saved to output.html" << std::endl;

	return 0;
}