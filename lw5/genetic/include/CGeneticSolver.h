#pragma once
#include "ISolver.h"

#include <random>

namespace knapsack
{

class CGeneticSolver : public ISolver
{
public:
	CGeneticSolver(int popSize, int gens, double mutRate);

	KnapsackResult Solve(const std::vector<Item>& items, int maxWeight) const override;

private:
	int m_populationSize;
	int m_generations;
	double m_mutationRate;
	mutable std::mt19937 m_rng;
};

} // namespace knapsack