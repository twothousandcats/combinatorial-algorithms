#include "Graph.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>

Graph::Graph(const int vertexCount)
	: m_vertexCount(vertexCount)
	  , m_timer(0)
{
	if (m_vertexCount < 0)
	{
		throw std::invalid_argument("Vertex count cannot be negative");
	}
	Clear();
}

// free memory and reset maps
void Graph::Clear()
{
	m_nameToIndex.clear();
	m_vertexNames.clear();

	if (m_vertexCount < 0)
	{
		m_vertexCount = 0;
	}

	m_adjMatrix.assign(m_vertexCount, std::vector<int>(m_vertexCount, 0));
	m_edgeList.clear();

	m_tin.assign(m_vertexCount, -1);
	m_low.assign(m_vertexCount, -1);
	m_onStack.assign(m_vertexCount, false);

	while (!m_stack.empty())
	{
		m_stack.pop();
	}
	m_timer = 0;
	m_sccs.clear();
}

int Graph::GetOrCreateIndex(const std::string& name)
{
	if (const auto it = m_nameToIndex.find(name); it != m_nameToIndex.end())
	{
		return it->second;
	}

	if (static_cast<int>(m_nameToIndex.size()) >= m_vertexCount)
	{
		throw std::runtime_error("Found more unique vertices than declared in header for name: " + name);
	}

	const int newIndex = static_cast<int>(m_nameToIndex.size());
	m_nameToIndex[name] = newIndex;
	m_vertexNames.push_back(name);

	return newIndex;
}

void Graph::BuildFromEdgeList(const EdgeList& edges, const int explicitVertexCount)
{
	if (explicitVertexCount > 0)
	{
		m_vertexCount = explicitVertexCount;
	}

	m_adjMatrix.assign(m_vertexCount, std::vector<int>(m_vertexCount, 0));
	m_edgeList = edges;

	for (const auto& [first, second] : m_edgeList)
	{
		if (first >= m_vertexCount || second >= m_vertexCount)
		{
			throw std::out_of_range("Vertex index out of bounds");
		}
		m_adjMatrix[first][second] = 1;
	}
}

void Graph::LoadFromFile(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + filename);
	}

	std::string line;
	if (!std::getline(file, line))
	{
		throw std::runtime_error("Empty file or missing header");
	}

	std::istringstream headerStream(line);
	int fileVertexCount;
	int edgeCount;
	if (!(headerStream >> fileVertexCount >> edgeCount))
	{
		throw std::runtime_error("Invalid file header format. Expected: <vertex_count> <edge_count>");
	}

	if (fileVertexCount <= 0 || edgeCount < 0)
	{
		throw std::invalid_argument("Invalid counts in file header");
	}

	m_vertexCount = fileVertexCount;
	Clear();

	// reading edges
	int linesRead = 0;
	while (linesRead < edgeCount && std::getline(file, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::istringstream lineStream(line);
		std::string uName, vName;

		if (!(lineStream >> uName >> vName))
		{
			throw std::runtime_error("Error reading edge at line " + std::to_string(linesRead + 2) + ": \"" + line + "\"");
		}

		try
		{
			int u = GetOrCreateIndex(uName);
			int v = GetOrCreateIndex(vName);

			// adding egde to matrix
			if (m_adjMatrix[u][v] == 0)
			{
				m_adjMatrix[u][v] = 1;
				m_edgeList.emplace_back(u, v);
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Error processing edge at line " + std::to_string(linesRead + 2) + ": " + e.what());
		}

		linesRead++;
	}

	if (linesRead != edgeCount)
	{
		throw std::runtime_error("File ended prematurely. Expected " + std::to_string(edgeCount) + " edges, found " + std::to_string(linesRead));
	}
}

void Graph::DfsTarjan(const int vertex)
{
	m_tin[vertex] = m_low[vertex] = m_timer++;
	m_stack.push(vertex);
	m_onStack[vertex] = true;

	for (int neighbor = 0; neighbor < m_vertexCount; ++neighbor)
	{
		if (m_adjMatrix[vertex][neighbor] == 0)
		{
			continue;
		}

		if (m_tin[neighbor] == -1)
		{
			DfsTarjan(neighbor);
			m_low[vertex] = std::min(m_low[vertex], m_low[neighbor]);
		}
		else if (m_onStack[neighbor])
		{
			m_low[vertex] = std::min(m_low[vertex], m_tin[neighbor]);
		}
	}

	if (m_low[vertex] == m_tin[vertex])
	{
		std::vector<int> component;
		while (true)
		{
			int v = m_stack.top();
			m_stack.pop();
			m_onStack[v] = false;
			component.push_back(v);

			if (v == vertex)
			{
				break;
			}
		}
		m_sccs.push_back(std::move(component));
	}
}

Graph::SCCList Graph::FindStronglyConnectedComponents()
{
	m_tin.assign(m_vertexCount, -1);
	m_low.assign(m_vertexCount, -1);
	m_onStack.assign(m_vertexCount, false);
	m_timer = 0;
	m_sccs.clear();
	while (!m_stack.empty())
	{
		m_stack.pop();
	}

	for (int i = 0; i < m_vertexCount; ++i)
	{
		if (m_tin[i] == -1)
		{
			DfsTarjan(i);
		}
	}

	return m_sccs;
}

int Graph::GetVertexCount() const
{
	return m_vertexCount;
}

const Graph::AdjMatrix& Graph::GetAdjacencyMatrix() const
{
	return m_adjMatrix;
}