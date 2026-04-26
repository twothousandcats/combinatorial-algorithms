#include "CFileLoader.h"
#include "ISolver.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>

using namespace knapsack;

void RunBenchmark(const std::string& name, const ISolver& solver, const std::vector<Item>& items, int maxWeight)
{
	const auto start = std::chrono::high_resolution_clock::now();
	const auto result = solver.Solve(items, maxWeight);
	const auto end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double, std::milli> elapsed = end - start;

	std::cout << "--- " << name << " ---\n";
	std::cout << "Time: " << std::fixed << std::setprecision(2) << elapsed.count() << " ms\n";
	PrintResult(std::cout, result);
	std::cout << "\n";
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: knapsack <input_file>\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	if (!file.is_open())
	{
		std::cerr << "Cannot open file: " << argv[1] << "\n";
		return 1;
	}

	const auto headerOpt = CFileLoader::LoadHeader(file);
	if (!headerOpt)
	{
		std::cerr << "Invalid header.\n";
		return 1;
	}

	std::vector<Item> items;
	if (!CFileLoader::LoadItems(file, headerOpt->itemCount, items))
	{
		std::cerr << "Failed to load items.\n";
		return 1;
	}

	const int maxWeight = headerOpt->maxWeight;
	// Brute Force (only for small N <= 30 as per request)
	if (items.size() <= 30)
	{
		const auto bfSolver = CreateBruteForceSolver();
		RunBenchmark("Brute Force", *bfSolver, items, maxWeight);
	}
	else
	{
		std::cout << "Skipping Brute Force (N > 30).\n\n";
	}

	// Genetic Algorithm
	// Tuned for larger inputs.
	// For N=1000, we need decent population and generations.
	const auto gaSolver = CreateGeneticSolver(200, 1000, 0.05);
	RunBenchmark("Genetic Algorithm", *gaSolver, items, maxWeight);

	return 0;
}