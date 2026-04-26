#include "CGeneticSolver.h"

#include "ISolver.h"

#include <random>

namespace knapsack
{
CGeneticSolver::CGeneticSolver(const int popSize, const int gens, const double mutRate)
	: m_populationSize(popSize)
	  , m_generations(gens)
	  , m_mutationRate(mutRate)
	  , m_rng(std::random_device{}())
{
	if (popSize <= 0 || gens <= 0)
		throw std::invalid_argument("Population size and generations must be positive");
}

KnapsackResult knapsack::CGeneticSolver::Solve(const std::vector<Item>& items, int maxWeight) const
{
	const size_t n = items.size();
	if (n == 0)
		return { -1, {}, false };

	// Chromosome: vector<bool> or vector<int>. Using vector<int> for simplicity in crossover
	using Chromosome = std::vector<int>;

	auto fitness = [&](const Chromosome& chromo) -> int {
		int w = 0, c = 0;
		for (size_t i = 0; i < n; ++i)
		{
			if (chromo[i])
			{
				w += items[i].weight;
				c += items[i].cost;
			}
		}
		// Penalty for exceeding weight
		if (w > maxWeight)
			return -1;
		return c;
	};

	auto createRandomChromosome = [&]() -> Chromosome {
		Chromosome chromo(n);
		std::uniform_int_distribution<int> dist(0, 1);
		for (auto& gene : chromo)
			gene = dist(m_rng);
		return chromo;
	};

	// Initialize population
	std::vector<Chromosome> population(m_populationSize);
	for (auto& chromo : population)
		chromo = createRandomChromosome();

	Chromosome bestChromo;
	int bestFit = -1;

	for (int gen = 0; gen < m_generations; ++gen)
	{
		// Evaluate
		std::vector<std::pair<int, size_t> > scores; // fitness, index
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

		// Selection (Tournament or Simple Top-K)
		// Let's use simple elitism + random selection for diversity
		std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
			return a.first > b.first;
		});

		std::vector<Chromosome> newPopulation;
		// Elitism: keep top 10%
		size_t eliteCount = std::max(size_t(1), m_populationSize / 10);
		for (size_t i = 0; i < eliteCount; ++i)
		{
			newPopulation.push_back(population[scores[i].second]);
		}

		// Crossover and Mutation
		std::uniform_real_distribution<double> probDist(0.0, 1.0);
		std::uniform_int_distribution<size_t> parentDist(0, eliteCount - 1); // Select from elite for stability
		std::uniform_int_distribution<size_t> crossDist(0, n - 1);

		while (newPopulation.size() < m_populationSize)
		{
			size_t p1Idx = parentDist(m_rng);
			size_t p2Idx = parentDist(m_rng);

			const Chromosome& p1 = population[scores[p1Idx].second];
			const Chromosome& p2 = population[scores[p2Idx].second];

			Chromosome child(n);
			size_t crossPoint = crossDist(m_rng);

			for (size_t i = 0; i < n; ++i)
			{
				child[i] = (i < crossPoint) ? p1[i] : p2[i];

				// Mutation
				if (probDist(m_rng) < m_mutationRate)
				{
					child[i] = !child[i];
				}
			}
			newPopulation.push_back(child);
		}

		population = std::move(newPopulation);
	}

	// Convert best chromosome to result
	KnapsackResult res;
	if (bestFit >= 0)
	{
		res.maxCost = bestFit;
		res.found = true;
		for (size_t i = 0; i < n; ++i)
		{
			if (bestChromo[i])
				res.bestIndices.push_back(i);
		}
	}
	else
	{
		res.found = false;
		res.maxCost = -1;
	}
	return res;
}
}