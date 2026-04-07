#include "Repository.h"
#include "BoruvkaTree.h"
#include "SteinerTree.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

std::vector<Terminal> ReadFromFile(const std::string& filePath)
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
	std::string filename = "input.txt";
	if (argc > 1)
	{
		filename = argv[1];
	}

	try
	{
		const auto terminals = ReadFromFile(filename);

		if (terminals.empty())
		{
			std::cerr << "No terminals found in file.\n";
			return 1;
		}

		Repository solver(
			std::make_unique<BoruvkaTree>(),
			std::make_unique<SteinerTree>()
		);

		const auto result = solver.Solve(terminals);

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