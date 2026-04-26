#include <catch2/catch_all.hpp>
#include "CFileLoader.h"
#include "ISolver.h"
#include "utils.h"
#include <vector>
#include <cmath>

using namespace knapsack;

TEST_CASE("Brute Force solves simple case", "[bruteforce]")
{
	std::vector<Item> items = {
		{ 10, 60 },
		{ 20, 100 },
		{ 30, 120 }
	};
	int maxWeight = 50;

	auto solver = CreateBruteForceSolver();
	auto result = solver->Solve(items, maxWeight);

	REQUIRE(result.found);
	REQUIRE(result.maxCost == 220); // Items 2 and 3 (100+120), weight 50
	REQUIRE(result.bestIndices.size() == 2);
}

TEST_CASE("Genetic Algorithm approximates simple case", "[genetic]")
{
	std::vector<Item> items = {
		{ 10, 60 },
		{ 20, 100 },
		{ 30, 120 }
	};
	int maxWeight = 50;

	// GA is probabilistic, so we run it with high params to ensure correctness for small N
	auto solver = CreateGeneticSolver(100, 500, 0.01);
	auto result = solver->Solve(items, maxWeight);

	REQUIRE(result.found);
	// GA should find optimal or very close for such small N
	REQUIRE(result.maxCost >= 200);
}

TEST_CASE("Empty items list", "[edge]")
{
	std::vector<Item> items;
	auto solver = CreateBruteForceSolver();
	auto result = solver->Solve(items, 10);
	REQUIRE(!result.found);
}

TEST_CASE("No item fits", "[edge]")
{
	std::vector<Item> items = { { 100, 50 } };
	auto solver = CreateBruteForceSolver();
	auto result = solver->Solve(items, 10);
	REQUIRE(!result.found);
}