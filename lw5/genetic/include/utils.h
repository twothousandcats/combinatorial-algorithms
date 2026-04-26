#pragma once

#include "ISolver.h";

namespace knapsack
{
std::unique_ptr<ISolver> CreateBruteForceSolver();

std::unique_ptr<ISolver> CreateGeneticSolver(int popSize, int gens, double mutRate);
}