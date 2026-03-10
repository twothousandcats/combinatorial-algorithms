#include "Graph.h"
#include <iostream>
#include <sstream>

Graph::Graph(const int vertexCount)
	: m_vertexCount(vertexCount > 0 ? vertexCount : 0),
	  m_timer(0)
{
	Clear();
}

void Graph::Clear()
{
	m_vertexNames.clear();
	m_nameToIndex.clear();
	m_adjMatrix.clear();
	m_adjList.clear();
	m_revAdjList.clear();
	m_visited.clear();
	m_sccs.clear();
	m_tin.clear();
	m_tout.clear();

	while (!m_orderStack.empty())
	{
		m_orderStack.pop();
	}
	m_vertexCount = 0;
	m_timer = 0;
}

int Graph::GetOrCreateIndex(const std::string& name)
{
	if (name.empty())
	{
		throw std::runtime_error("Ошибка: Пустое имя вершины");
	}

	if (const auto it = m_nameToIndex.find(name); it != m_nameToIndex.end())
	{
		return it->second;
	}

	if (m_vertexCount > 0 && static_cast<int>(m_nameToIndex.size()) >= m_vertexCount)
	{
		throw std::runtime_error("Ошибка: Превышено количество уникальных вершин над заявленным");
	}

	const int newIndex = static_cast<int>(m_nameToIndex.size());
	m_nameToIndex[name] = newIndex;

	// Resize storage
	if (static_cast<int>(m_vertexNames.size()) <= newIndex)
	{
		const int newSize = newIndex + 1;
		m_vertexNames.resize(newSize);
		if (m_vertexCount == 0 || newSize > m_vertexCount)
		{
			// Adjust matrix size dynamically if initial count was 0 or insufficient
			m_adjMatrix.resize(newSize, std::vector<int>(newSize, 0));
			m_adjList.resize(newSize);
			m_revAdjList.resize(newSize);
		}
	}

	m_vertexNames[newIndex] = name;
	return newIndex;
}

void Graph::LoadFromFile(const std::string& filename)
{
	Clear();
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Ошибка: Не удалось открыть файл " + filename);
	}

	if (int declaredEdges = 0; !(file >> m_vertexCount >> declaredEdges))
	{
		throw std::runtime_error("Ошибка: Неверный формат заголовка файла");
	}

	if (m_vertexCount <= 0)
	{
		throw std::runtime_error("Ошибка: Количество вершин должно быть положительным");
	}

	// Pre-allocate
	m_vertexNames.resize(m_vertexCount);
	m_adjMatrix.assign(m_vertexCount, std::vector<int>(m_vertexCount, 0));
	m_adjList.resize(m_vertexCount);
	m_revAdjList.resize(m_vertexCount);

	std::string uName, vName;
	while (file >> uName >> vName)
	{
		int u = GetOrCreateIndex(uName);
		int v = GetOrCreateIndex(vName);

		if (u >= m_vertexCount || v >= m_vertexCount)
		{
			throw std::runtime_error("Ошибка: Индекс вершины выходит за пределы объявленного количества");
		}

		if (m_adjMatrix[u][v] == 0)
		{
			m_adjMatrix[u][v] = 1;
			m_adjList[u].push_back(v);
			m_revAdjList[v].push_back(u);
		}
	}

	if (static_cast<int>(m_nameToIndex.size()) != m_vertexCount)
	{
		throw std::runtime_error("Ошибка: Количество найденных вершин не совпадает с заголовком");
	}

	ValidateMatrix();
}

void Graph::ValidateMatrix() const
{
	// square matrix
	for (const auto& row : m_adjMatrix)
	{
		if (row.size() != static_cast<size_t>(m_vertexCount))
		{
			throw std::runtime_error("Ошибка: Матрица смежности не квадратная или повреждена");
		}
	}
}

Graph::EdgeList Graph::ConvertMatrixToEdgeList() const
{
	EdgeList edges;
	for (int i = 0; i < m_vertexCount; ++i)
	{
		for (int j = 0; j < m_vertexCount; ++j)
		{
			if (m_adjMatrix[i][j] == 1)
			{
				edges.emplace_back(i, j);
			}
		}
	}
	return edges;
}

void Graph::BuildFromEdgeList(const EdgeList& edges, const int explicitVertexCount)
{
	Clear();
	if (explicitVertexCount > 0)
	{
		m_vertexCount = explicitVertexCount;
		m_vertexNames.resize(m_vertexCount);
		m_adjMatrix.assign(m_vertexCount, std::vector<int>(m_vertexCount, 0));
		m_adjList.resize(m_vertexCount);
		m_revAdjList.resize(m_vertexCount);

		for (const auto& edge : edges)
		{
			if (edge.first >= m_vertexCount || edge.second >= m_vertexCount)
			{
				throw std::runtime_error("Ошибка: Индекс ребра вне диапазона");
			}
			if (m_adjMatrix[edge.first][edge.second] == 0)
			{
				m_adjMatrix[edge.first][edge.second] = 1;
				m_adjList[edge.first].push_back(edge.second);
				m_revAdjList[edge.second].push_back(edge.first);
			}
		}
	}
	else
	{
		// Dynamic build similar from pairs
		for (const auto& edge : edges)
		{
			int maxIdx = std::max(edge.first, edge.second);
			if (maxIdx >= m_vertexCount)
			{
				int newCount = maxIdx + 1;
				m_vertexCount = newCount;
				m_vertexNames.resize(newCount);
				m_adjMatrix.resize(newCount, std::vector(newCount, 0));
				m_adjList.resize(newCount);
				m_revAdjList.resize(newCount);
			}
			if (m_adjMatrix[edge.first][edge.second] == 0)
			{
				m_adjMatrix[edge.first][edge.second] = 1;
				m_adjList[edge.first].push_back(edge.second);
				m_revAdjList[edge.second].push_back(edge.first);
			}
		}
	}
	ValidateMatrix();
}

void Graph::DfsFillOrder(const int vertex)
{
	m_visited[vertex] = true;
	m_tin[vertex] = ++m_timer;

	for (const int neighbor : m_adjList[vertex])
	{
		if (!m_visited[neighbor])
		{
			DfsFillOrder(neighbor);
		}
	}

	m_tout[vertex] = ++m_timer;
	m_orderStack.push(vertex);
}

void Graph::DfsCollectComponent(const int vertex, IntVector& component)
{
	m_visited[vertex] = true;
	component.push_back(vertex);
	for (const int neighbor : m_revAdjList[vertex])
	{
		if (!m_visited[neighbor])
		{
			DfsCollectComponent(neighbor, component);
		}
	}
}

Graph::SCCList Graph::FindStronglyConnectedComponents()
{
	m_sccs.clear();
	m_orderStack = std::stack<int>();
	m_timer = 0;

	// alloc
	m_visited.assign(m_vertexCount, false);
	m_tin.assign(m_vertexCount, 0);
	m_tout.assign(m_vertexCount, 0);

	// Fill order stack
	for (int i = 0; i < m_vertexCount; ++i)
	{
		if (!m_visited[i])
		{
			DfsFillOrder(i);
		}
	}

	// Process in reverse order
	m_visited.assign(m_vertexCount, false);
	while (!m_orderStack.empty())
	{
		const int vertex = m_orderStack.top();
		m_orderStack.pop();
		if (!m_visited[vertex])
		{
			IntVector component;
			DfsCollectComponent(vertex, component);
			m_sccs.push_back(component);
		}
	}

	return m_sccs;
}

int Graph::GetVertexCount() const { return m_vertexCount; }
const Graph::AdjMatrix& Graph::GetAdjacencyMatrix() const { return m_adjMatrix; }
const Graph::StringVector& Graph::GetVertexNames() const { return m_vertexNames; }