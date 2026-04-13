#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Geometry.h"
#include "Graph.h"
#include "MstBoruvka.h"
#include "SteinerTree.h"
#include "Visualizer.h"

bool LoadTerminalsFromFile(const std::string& filename, Graph& terminals)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::istringstream iss(line);
		std::string label;
		if (double x, y; iss >> label >> x >> y)
		{
			terminals.AddNode(geometry::Point(x, y));
		}
		else
		{
			std::cerr << "Warning: Skipping invalid line: " << line << std::endl;
		}
	}

	file.close();
	return true;
}

int main(int argc, char* argv[])
{
	Graph terminals;

	// Load Terminals from file
	std::string inputFilename = "input.txt";
	if (argc > 1)
	{
		inputFilename = argv[1];
	}

	if (!LoadTerminalsFromFile(inputFilename, terminals))
	{
		return 1;
	}

	const std::size_t N = terminals.GetNodeCount();
	if (N < 2)
	{
		std::cerr << "Error: Need at least 2 points to compute MST/Steiner tree." << std::endl;
		return 1;
	}

	std::cout << "Loaded " << N << " terminals from file." << std::endl;

	// Compute MST
	const Graph mst = MstBoruvka::Compute(terminals);
	const double mstLen = mst.TotalWeight();

	// Compute Steiner Tree
	const auto steinerRes = SteinerTreeSolver::Compute(terminals);
	const double steinerLen = steinerRes.length;

	// Output Results
	std::cout << "MST Length (Boruvka): " << mstLen << std::endl;
	std::cout << "Steiner Length (Heuristic): " << steinerLen << std::endl;

	if (mstLen > 0)
	{
		std::cout << "Improvement: " << ((mstLen - steinerLen) / mstLen * 100.0) << "%" << std::endl;
	}
	else
	{
		std::cout << "Improvement: N/A (MST length is 0)" << std::endl;
	}

	std::cout << "Steiner Points Added: " << steinerRes.steinerPointsCount << std::endl;

	// Visualize
	Visualizer::SaveToHtml(mst, steinerRes.graph, "output.html");
	std::cout << "Visualization saved to output.html" << std::endl;

	return 0;
}