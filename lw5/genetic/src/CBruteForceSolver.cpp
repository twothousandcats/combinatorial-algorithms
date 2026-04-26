#include "ISolver.h"

#include <vector>

namespace knapsack
{
bool NextPermutation(std::vector<int>& bits)
{
	for (auto it = bits.rbegin(); it != bits.rend(); ++it)
	{
		if (*it == 0)
		{
			*it = 1;
			return true;
		}
		*it = 0;
	}
	return false;
}

KnapsackResult Solve(const std::vector<Item>& items, const int maxWeight)
{
	const size_t n = items.size();
	if (n == 0)
		return { -1, {}, false };

	// Use int vector for easier manipulation than size_t for bits
	std::vector<int> current(n, 0);
	KnapsackResult best{ -1, {}, false };

	do
	{
		int curWeight = 0;
		int curCost = 0;
		for (size_t i = 0; i < n; ++i)
		{
			if (current[i] == 1)
			{
				curWeight += items[i].weight;
				curCost += items[i].cost;
			}
		}

		if (curWeight <= maxWeight && curCost > best.maxCost)
		{
			best.maxCost = curCost;
			best.found = true;
			best.bestIndices.clear();
			for (size_t i = 0; i < n; ++i)
			{
				if (current[i] == 1)
					best.bestIndices.push_back(i);
			}
		}
	} while (NextPermutation(current));

	return best;
}
}