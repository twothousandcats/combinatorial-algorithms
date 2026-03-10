#pragma once

#include <vector>
#include <string>
#include <stack>
#include <utility>
#include <unordered_map>
#include <sstream>

class Graph
{
public:
	using AdjMatrix = std::vector<std::vector<int> >;
	using EdgeList = std::vector<std::pair<int, int> >;
	using SCCList = std::vector<std::vector<int> >;

	// Tarjan algorithm vectors
	using IntVector = std::vector<int>;
	using BoolVector = std::vector<bool>;

	explicit Graph(int vertexCount = 0); // 0 by default

	void LoadFromFile(const std::string& filename);

	[[nodiscard]] SCCList FindStronglyConnectedComponents();

	[[nodiscard]] int GetVertexCount() const;

	[[nodiscard]] const AdjMatrix& GetAdjacencyMatrix() const;

	// Helper to get name by index
	[[nodiscard]] const std::vector<std::string>& GetVertexNames() const { return m_vertexNames; }

private:
	int m_vertexCount;
	std::vector<std::string> m_vertexNames; // Index -> Name mapping
	std::unordered_map<std::string, int> m_nameToIndex; // Name -> Index mapping

	AdjMatrix m_adjMatrix;
	EdgeList m_edgeList;

	// Tarjan state
	IntVector m_tin; // first visit [v]
	IntVector m_low; // min(m_tin[v])
	BoolVector m_onStack;
	std::stack<int> m_stack;
	int m_timer;
	SCCList m_sccs;

	void Clear();

	void BuildFromEdgeList(const EdgeList& edges, int explicitVertexCount = -1);

	void DfsTarjan(int vertex);

	int GetOrCreateIndex(const std::string& name);
};