#include "CFileLoader.h"
#include "ISolver.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>

namespace
{

constexpr int POPULATION_COUNT = 200;
constexpr int GENERATIONS_COUNT = 1000;
constexpr double MUTATIONS_FACTOR = 0.01;
constexpr int GREEDY_MAX_ITEMS = 12;

void RunBenchmark(const std::string& name, const knapsack::ISolver& solver, const std::vector<knapsack::Item>& items, int maxWeight)
{
	const auto start = std::chrono::high_resolution_clock::now();
	const auto result = solver.Solve(items, maxWeight);
	const auto end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double, std::milli> elapsed = end - start;

	std::cout << "" << name << ":\n";
	std::cout << "Time: " << std::fixed << std::setprecision(2) << elapsed.count() << " ms\n";
	knapsack::PrintResult(std::cout, result);
	std::cout << "\n";
}

} // namespace

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

	const auto headerOpt = knapsack::CFileLoader::LoadHeader(file);
	if (!headerOpt)
	{
		std::cerr << "Invalid header.\n";
		return 1;
	}

	std::vector<knapsack::Item> items;
	if (!knapsack::CFileLoader::LoadItems(file, headerOpt->itemCount, items))
	{
		std::cerr << "Failed to load items.\n";
		return 1;
	}

	const int maxWeight = headerOpt->maxWeight;

	// Greedy Algorithm with limit of 12 items
	{
		// Create a copy to avoid modifying the original 'items' vector used by Genetic Algorithm
		std::vector<knapsack::Item> itemsForGreedy = items;

		if (itemsForGreedy.size() > GREEDY_MAX_ITEMS)
		{
			itemsForGreedy.resize(GREEDY_MAX_ITEMS);
		}

		if (const auto greedySolver = knapsack::CreateBruteForceSolver())
		{
			RunBenchmark("Greedy Algorithm (Limited to 12)", *greedySolver, itemsForGreedy, maxWeight);
		}
		else
		{
			std::cerr << "Greedy solver not available.\n\n";
		}
	}

	// Genetic Algorithm
	// Tuned for larger inputs
	// For N=1000, we need decent population and generations
	const auto gaSolver = knapsack::CreateGeneticSolver(
		POPULATION_COUNT,
		GENERATIONS_COUNT,
		MUTATIONS_FACTOR
		);
	RunBenchmark("Genetic Algorithm", *gaSolver, items, maxWeight);

	return 0;
}