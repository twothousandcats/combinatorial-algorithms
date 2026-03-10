#pragma once

#include <vector>
#include <string>
#include <stack>
#include <utility>
#include <unordered_map>
#include <fstream>

class Graph
{
public:
	using AdjMatrix = std::vector<std::vector<int> >;
	using AdjList = std::vector<std::vector<int> >;

	using EdgeList = std::vector<std::pair<int, int> >;
	using SCCList = std::vector<std::vector<int> >;

	using IntVector = std::vector<int>;
	using BoolVector = std::vector<bool>;
	using StringVector = std::vector<std::string>;

	explicit Graph(int vertexCount = 0);

	void LoadFromFile(const std::string& filename);

	[[nodiscard]] SCCList FindStronglyConnectedComponents();

	[[nodiscard]] const StringVector& GetVertexNames() const;

	[[nodiscard]] int GetVertexCount() const;

	[[nodiscard]] const AdjMatrix& GetAdjacencyMatrix() const;

	// Adapters
	[[nodiscard]] EdgeList ConvertMatrixToEdgeList() const;

	void BuildFromEdgeList(const EdgeList& edges, int explicitVertexCount = -1);

private:
	void Clear();

	int GetOrCreateIndex(const std::string& name);

	void DfsFillOrder(int vertex);

	void DfsCollectComponent(int vertex, IntVector& component);

	void ValidateMatrix() const;

	int m_vertexCount;
	StringVector m_vertexNames; // Index -> Name
	std::unordered_map<std::string, int> m_nameToIndex; // Name -> Index

	AdjMatrix m_adjMatrix;
	AdjList m_adjList; // forward DFS
	AdjList m_revAdjList; // reverse DFS (transposed)

	// State for Kosaraju
	BoolVector m_visited;
	std::stack<int> m_orderStack;
	SCCList m_sccs;

	// times
	IntVector m_tin;
	IntVector m_tout;
	int m_timer;
};