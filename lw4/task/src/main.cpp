#include "Repository.h"
#include "BoruvkaTree.h"
#include "SteinerTree.h"
#include <iostream>
#include <string>

ReadFromFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file: " + filePath);
	}

	std::vector<Terminal> terminals;
	std::string line;

	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		std::istringstream iss(line);
		std::string name;
		int xInt{};
		int yInt{};

		if (!(iss >> name >> xInt >> yInt))
		{
			// Skip malformed lines or throw depending on requirements
			continue;
		}

		terminals.push_back({
			name,
			Point(static_cast<double>(xInt), static_cast<double>(yInt))
		});
	}

	return terminals;
}

int main(int argc, char* argv[])
{
	// Default file or from argument
	std::string filename = "input.txt";
	if (argc > 1)
	{
		filename = argv[1];
	}

	try
	{
		// 1. Read Data
		const auto terminals = ReadFromFile(filename);

		if (terminals.empty())
		{
			std::cerr << "No terminals found in file.\n";
			return 1;
		}

		// 2. Setup Solver with Dependencies
		auto solver = NetworkSolver(
			std::make_unique<BoruvkaMst>(),
			std::make_unique<SteinerHeuristic>()
		);

		// 3. Solve
		const auto result = solver.Solve(terminals);

		// 4. Output
		std::cout << "Results:\n";
		std::cout << "MST Length:      " << result.mstLength << "\n";
		std::cout << "Steiner Length:  " << result.steinerLength << "\n";
		std::cout << "Ratio (MST/SMT): " << result.ratio << "\n";
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}