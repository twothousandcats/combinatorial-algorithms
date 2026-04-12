#include "Graph.h"

namespace
{

constexpr double Epsilon = 1e-9;

} // namespace

Edge::Edge(int u, int v, double w)
	: u(u), v(v), weight(w)
{
}

void Graph::AddNode(const geometry::Point& p)
{
	nodes.push_back(p);
}

void Graph::AddEdge(int u, int v)
{
	if (u < 0 || v < 0 || u >= static_cast<int>(nodes.size()) || v >= static_cast<int>(nodes.size()))
	{
		return;
	}
	double w = geometry::Distance(nodes[u], nodes[v]);
	edges.emplace_back(u, v, w);
}

double Graph::TotalWeight() const
{
	double sum = 0.0;
	for (const auto& e : edges)
	{
		sum += e.weight;
	}
	return sum;
}