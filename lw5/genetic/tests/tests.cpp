#include <catch2/catch_all.hpp>
#include "CFileLoader.h"
#include "ISolver.h"
#include "utils.h"

#include <vector>
#include <cmath>

namespace
{

constexpr double MUTATION_RATE = 0.01;
constexpr int SMALL_POPULATION = 100;
constexpr int SMALL_GENERATIONS = 500;

constexpr int LARGE_POPULATION = 500;
constexpr int LARGE_GENERATIONS = 2000;

} // namespace

TEST_CASE("Brute Force solves simple case", "[bruteforce]")
{
	const std::vector<knapsack::Item> items = {
		{ 10, 60 },
		{ 20, 100 },
		{ 30, 120 }
	};
	constexpr int maxWeight = 50;

	auto solver = knapsack::CreateBruteForceSolver();
	const auto result = solver->Solve(items, maxWeight);

	REQUIRE(result.found);
	REQUIRE(result.maxCost == 220); // Items 2 and 3 (100+120), weight 50
	REQUIRE(result.bestIndices.size() == 2);
}

TEST_CASE("Genetic Algorithm approximates simple case", "[genetic]")
{
	const std::vector<knapsack::Item> items = {
		{ 10, 60 },
		{ 20, 100 },
		{ 30, 120 }
	};
	constexpr int maxWeight = 50;

	// GA is probabilistic, so we run it with high params to ensure correctness for small N
	auto solver = knapsack::CreateGeneticSolver(
		SMALL_POPULATION, SMALL_GENERATIONS, MUTATION_RATE);
	const auto result = solver->Solve(items, maxWeight);

	REQUIRE(result.found);
	// GA should find optimal or very close for such small N
	REQUIRE(result.maxCost >= 200);
}

TEST_CASE("Empty items list", "[edge]")
{
	const std::vector<knapsack::Item> items;
	auto solver = knapsack::CreateBruteForceSolver();
	const auto result = solver->Solve(items, 10);
	REQUIRE(!result.found);
}

TEST_CASE("No item fits", "[edge]")
{
	const std::vector<knapsack::Item> items = { { 100, 50 } };
	auto solver = knapsack::CreateBruteForceSolver();
	const auto result = solver->Solve(items, 10);
	REQUIRE(result.found);
}

TEST_CASE("Genetic Algorithm handles large input (>=1000 items)", "[genetic][large]")
{
	// Generate 1000 items with random weights and costs
	std::vector<knapsack::Item> items;
	items.reserve(1000);
	for (int i = 0; i < 1000; ++i)
	{
		items.push_back({ (i % 50) + 1, (i % 100) + 10 });
	}
	constexpr int maxWeight = 5000;

	const auto solver = knapsack::CreateGeneticSolver(
		LARGE_POPULATION, LARGE_GENERATIONS, MUTATION_RATE);
	const auto result = solver->Solve(items, maxWeight);

	// GA should find a valid solution
	REQUIRE(result.found);
	REQUIRE(result.maxCost > 0);
}

/* TEST_CASE("Brute Force and Genetic produce similar results on small input", "[comparison]")
{
	const std::vector<knapsack::Item> items = {
		{ 5, 30 },
		{ 10, 40 },
		{ 15, 45 },
		{ 20, 70 },
		{ 25, 80 }
	};
	constexpr int maxWeight = 50;

	auto bfSolver = knapsack::CreateBruteForceSolver();
	const auto bfResult = bfSolver->Solve(items, maxWeight);

	auto gaSolver = knapsack::CreateGeneticSolver(
		LARGE_POPULATION, LARGE_GENERATIONS, MUTATION_RATE);
	const auto gaResult = gaSolver->Solve(items, maxWeight);

	REQUIRE(bfResult.found);
	REQUIRE(gaResult.found);
	// GA should find optimal or very close
	REQUIRE(gaResult.maxCost >= bfResult.maxCost * 0.9); // At least 90% of optimal
} */