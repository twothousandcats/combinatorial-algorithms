#pragma once

#include "Edge.h"
#include "Point.h"

#include <vector>

namespace steiner
{

class Graph
{
public:
	std::size_t AddVertex(Point point);

	void AddEdge(std::size_t from, std::size_t to);

	const std::vector<Point>& GetVertices() const noexcept { return m_vertices; }
	const std::vector<Edge>& GetEdges() const noexcept { return m_edges; }
	std::size_t GetVertexCount() const noexcept { return m_vertices.size(); }

	// Builds a complete undirected weighted graph over the given vertices.
	// Edge weights are Euclidean distances.
	static Graph BuildComplete(std::vector<Point> vertices);

private:
	std::vector<Point> m_vertices;
	std::vector<Edge> m_edges;
};

double TotalWeight(const std::vector<Edge>& edges) noexcept;

} // namespace steiner