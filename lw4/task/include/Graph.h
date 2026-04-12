#pragma once
#include "Geometry.h"
#include <vector>
#include <memory>

struct Edge
{
	int u{};
	int v{};
	double weight{};

	Edge() = default;

	Edge(int u, int v, double w);
};

struct Graph
{
	std::vector<geometry::Point> nodes;
	std::vector<Edge> edges;

	void AddNode(const geometry::Point& p);

	void AddEdge(int u, int v);

	double TotalWeight() const;
};