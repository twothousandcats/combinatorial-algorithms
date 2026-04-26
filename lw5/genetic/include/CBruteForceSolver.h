#pragma once
#include "ISolver.h"

namespace knapsack
{

class CBruteForceSolver : public ISolver
{
public:
	KnapsackResult Solve(const std::vector<Item>& items, int maxWeight) const override;
};

} // namespace knapsack