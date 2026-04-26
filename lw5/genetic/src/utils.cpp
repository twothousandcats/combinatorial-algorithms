#include "CBruteForceSolver.h"
#include "CGeneticSolver.h"

#include <memory>

namespace knapsack
{

std::unique_ptr<ISolver> CreateBruteForceSolver()
{
	return std::make_unique<CBruteForceSolver>();
}

std::unique_ptr<ISolver> CreateGeneticSolver(int popSize, int gens, double mutRate)
{
	return std::make_unique<CGeneticSolver>(popSize, gens, mutRate);
}

} // namespace knapsack