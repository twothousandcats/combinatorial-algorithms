#include "Graph.h"

#include <numeric>
#include <stdexcept>
#include <utility>

namespace steiner
{

std::size_t Graph::AddVertex(Point point)
{
	m_vertices.push_back(std::move(point));
	return m_vertices.size() - 1;
}

void Graph::AddEdge(std::size_t from, std::size_t to)
{
	if (from >= m_vertices.size() || to >= m_vertices.size())
	{
		throw std::out_of_range{ "Graph::AddEdge: vertex index out of range" };
	}
	Edge edge{};
	edge.from = from;
	edge.to = to;
	edge.weight = Distance(m_vertices[from], m_vertices[to]);
	m_edges.push_back(edge);
}

Graph Graph::BuildComplete(std::vector<Point> vertices)
{
	Graph graph;
	graph.m_vertices = std::move(vertices);
	const std::size_t n = graph.m_vertices.size();
	if (n >= 2)
	{
		graph.m_edges.reserve(n * (n - 1) / 2);
	}
	for (std::size_t i = 0; i < n; ++i)
	{
		for (std::size_t j = i + 1; j < n; ++j)
		{
			Edge edge{};
			edge.from = i;
			edge.to = j;
			edge.weight = Distance(graph.m_vertices[i], graph.m_vertices[j]);
			graph.m_edges.push_back(edge);
		}
	}
	return graph;
}

double TotalWeight(const std::vector<Edge>& edges) noexcept
{
	return std::accumulate(edges.begin(), edges.end(), 0.0,
		[](double sum, const Edge& e) { return sum + e.weight; });
}

} // namespace steiner