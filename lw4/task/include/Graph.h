#pragma once
#include "Geometry.h"
#include "Edge.h"
#include <vector>

struct Graph
{
	std::vector<Edge> edges;
	std::vector<geometry::Point> nodes;

	void AddNode(const geometry::Point& p);

	void AddEdge(int u, int v);

	double TotalWeight() const;

	[[nodiscard]] std::size_t GetNodeCount() const;
};