#include "CGeneticSolver.h"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <numeric>

namespace knapsack
{

CGeneticSolver::CGeneticSolver(const int popSize, const int gens, const double mutRate)
	: m_populationSize(popSize)
	  , m_generations(gens)
	  , m_mutationRate(mutRate)
	  , m_rng(std::random_device{}())
{
	if (popSize <= 0 || gens <= 0)
	{
		throw std::invalid_argument("Population size and generations must be positive");
	}
}

KnapsackResult CGeneticSolver::Solve(const std::vector<Item>& items, const int maxWeight) const
{
	const size_t count = items.size();
	if (count == 0)
	{
		return { -1, {}, false };
	}

	using Chromosome = std::vector<int>;

	auto evaluate = [&](const Chromosome& chromo) -> std::pair<int, int> {
		// weight, cost
		int weight = 0;
		int cost = 0;
		for (size_t i = 0; i < count; ++i)
		{
			if (chromo[i])
			{
				weight += items[i].weight;
				cost += items[i].cost;
			}
		}
		return { weight, cost };
	};

	// If there is excess weight, then there is a fine
	auto fitness = [&](const Chromosome& chromo) -> int {
		auto [weight, cost] = evaluate(chromo);
		if (weight > maxWeight)
		{
			return -1;
		}
		return cost;
	};

	// Generation of a valid chromosome
	auto createRandomChromosome = [&]() -> Chromosome {
		Chromosome chromo(count, 0);
		std::vector<size_t> indices(count);
		std::iota(indices.begin(), indices.end(), 0);
		std::ranges::shuffle(indices, m_rng);

		int currentWeight = 0;
		for (const size_t idx : indices)
		{
			if (currentWeight + items[idx].weight <= maxWeight)
			{
				chromo[idx] = 1;
				currentWeight += items[idx].weight;
			}
		}
		return chromo;
	};

	std::vector<Chromosome> population(m_populationSize);
	for (auto& chromo : population)
	{
		// use improved initialization to guarantee at least one valid solution.
		chromo = createRandomChromosome();
	}

	Chromosome bestChromo;
	int bestFit = -1;

	// distributions
	std::uniform_real_distribution<double> probDist(0.0, 1.0);
	std::uniform_int_distribution<size_t> crossDist(0, count - 1);

	// Tournament selection
	const size_t selectionPoolSize = std::max(static_cast<size_t>(2), static_cast<size_t>(m_populationSize / 2));

	for (int gen = 0; gen < m_generations; ++gen)
	{
		std::vector<std::pair<int, size_t> > scores;
		scores.reserve(m_populationSize);

		int currentGenBestFit = -1;
		size_t currentGenBestIdx = 0;

		for (size_t i = 0; i < population.size(); ++i)
		{
			int fit = fitness(population[i]);
			scores.emplace_back(fit, i);
			if (fit > currentGenBestFit)
			{
				currentGenBestFit = fit;
				currentGenBestIdx = i;
			}
		}

		if (currentGenBestFit > bestFit)
		{
			bestFit = currentGenBestFit;
			bestChromo = population[currentGenBestIdx];
		}

		std::ranges::sort(scores, [](const auto& a, const auto& b) {
			return a.first > b.first;
		});

		std::vector<Chromosome> newPopulation;
		// Elitism
		const size_t eliteCount = std::max(static_cast<size_t>(1), static_cast<size_t>(m_populationSize / 10));
		for (size_t i = 0; i < eliteCount; ++i)
		{
			newPopulation.push_back(population[scores[i].second]);
		}

		while (newPopulation.size() < m_populationSize)
		{
			std::uniform_int_distribution<size_t> parentDist(0, selectionPoolSize - 1);

			size_t p1Idx = scores[parentDist(m_rng)].second;
			size_t p2Idx = scores[parentDist(m_rng)].second;

			const Chromosome& p1 = population[p1Idx];
			const Chromosome& p2 = population[p2Idx];

			Chromosome child(count);
			const size_t crossPoint = crossDist(m_rng);
			for (size_t i = 0; i < count; ++i)
			{
				child[i] = (i < crossPoint) ? p1[i] : p2[i];
			}
			for (size_t i = 0; i < count; ++i)
			{
				if (probDist(m_rng) < m_mutationRate)
				{
					child[i] = !child[i];
				}
			}
			newPopulation.push_back(child);
		}

		population = std::move(newPopulation);
	}

	KnapsackResult res;
	if (bestFit >= 0)
	{
		res.maxCost = bestFit;
		res.found = true;
		for (size_t i = 0; i < count; ++i)
		{
			if (bestChromo[i])
			{
				res.bestIndices.push_back(i);
			}
		}
	}
	else
	{
		res.found = false;
		res.maxCost = -1;
	}
	return res;
}

} // namespace knapsack