#pragma once
#include <vector>
#include <cstddef>
#include <string>

namespace knapsack
{

struct Item
{
	int weight = 0;
	int cost = 0;
};

struct KnapsackResult
{
	int maxCost = -1;
	std::vector<size_t> bestIndices; // Indices of selected items
	bool found = false;
};

class ISolver
{
public:
	virtual ~ISolver() = default;

	virtual KnapsackResult Solve(const std::vector<Item>& items, int maxWeight) const = 0;
};

} // namespace knapsack