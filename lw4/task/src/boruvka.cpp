#include "steiner_lab/boruvka.h"

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <optional>
#include <vector>

namespace steiner_lab
{

// Инициализирует DSU одиночными компонентами и нужен для старта Борувки.
BoruvkaMstSolver::DisjointSetUnion::DisjointSetUnion(std::size_t elementCount) : m_parent(elementCount), m_rank(elementCount, 0)
{
	std::iota(m_parent.begin(), m_parent.end(), 0);
}

// Находит корень компоненты с сжатием пути и нужен для быстрых проверок связности.
int BoruvkaMstSolver::DisjointSetUnion::Find(int elementIndex)
{
	const std::size_t slot = static_cast<std::size_t>(elementIndex);
	if (m_parent[slot] != elementIndex)
	{
		m_parent[slot] = Find(m_parent[slot]);
	}
	return m_parent[slot];
}

// Подвешивает корень-потомок к корню-родителю и нужен для объединения компонент.
void BoruvkaMstSolver::DisjointSetUnion::AttachChildToParent(int childRoot, int parentRoot)
{
	m_parent[static_cast<std::size_t>(childRoot)] = parentRoot;
}

// Увеличивает ранг родителя при равных рангах и нужен для балансировки DSU.
void BoruvkaMstSolver::DisjointSetUnion::BumpParentRankIfTied(int parentRoot, bool siblingRanksWereEqual)
{
	if (siblingRanksWereEqual)
	{
		++m_rank[static_cast<std::size_t>(parentRoot)];
	}
}

// Объединяет две компоненты по рангу и нужен для построения ацикличного остова.
bool BoruvkaMstSolver::DisjointSetUnion::Unite(int firstIndex, int secondIndex)
{
	int rootFirst = Find(firstIndex);
	int rootSecond = Find(secondIndex);
	if (rootFirst == rootSecond)
	{
		return false;
	}
	if (m_rank[static_cast<std::size_t>(rootFirst)] < m_rank[static_cast<std::size_t>(rootSecond)])
	{
		std::swap(rootFirst, rootSecond);
	}
	const bool ranksEqual = (m_rank[static_cast<std::size_t>(rootFirst)] == m_rank[static_cast<std::size_t>(rootSecond)]);
	AttachChildToParent(rootSecond, rootFirst);
	BumpParentRankIfTied(rootFirst, ranksEqual);
	return true;
}

// Считает вес ребра между терминалами и нужен для оценки кандидатов Борувки.
double BoruvkaMstSolver::TerminalEdgeWeight(const std::vector<Point2D>& terminals, int firstIndex, int secondIndex)
{
	return EuclideanGeometry::Distance(terminals[static_cast<std::size_t>(firstIndex)],
		terminals[static_cast<std::size_t>(secondIndex)]);
}

namespace
{
// Обновляет лучшее исходящее ребро компоненты и нужен для шага выбора в фазе Борувки.
void RelaxBestEdgeForRoot(std::vector<std::optional<UndirectedEdge>>& bestForRoot, int root, const UndirectedEdge& edge)
{
	const std::size_t slot = static_cast<std::size_t>(root);
	if (!bestForRoot[slot] || edge.weight < (*bestForRoot[slot]).weight)
	{
		bestForRoot[slot] = edge;
	}
}
} // namespace

// Рассматривает ребро для двух компонент и нужен для параллельного обновления их минимумов.
void BoruvkaMstSolver::ConsiderEdgeForComponentMinima(std::vector<std::optional<UndirectedEdge>>& bestForRoot, int rootFirst,
	int rootSecond, const UndirectedEdge& edge)
{
	RelaxBestEdgeForRoot(bestForRoot, rootFirst, edge);
	RelaxBestEdgeForRoot(bestForRoot, rootSecond, edge);
}

// Собирает минимальные исходящие ребра по всем компонентам и нужен для одной фазы Борувки.
std::vector<std::optional<UndirectedEdge>> BoruvkaMstSolver::CollectPhaseMinimumEdges(
	const std::vector<Point2D>& terminals, int terminalCount, DisjointSetUnion& disjointSet)
{
	std::vector<std::optional<UndirectedEdge>> bestForRoot(static_cast<std::size_t>(terminalCount));
	for (int firstIndex = 0; firstIndex < terminalCount; ++firstIndex)
	{
		for (int secondIndex = firstIndex + 1; secondIndex < terminalCount; ++secondIndex)
		{
			const int rootFirst = disjointSet.Find(firstIndex);
			const int rootSecond = disjointSet.Find(secondIndex);
			if (rootFirst == rootSecond)
			{
				continue;
			}
			const double weight = TerminalEdgeWeight(terminals, firstIndex, secondIndex);
			const UndirectedEdge candidateEdge{firstIndex, secondIndex, weight};
			ConsiderEdgeForComponentMinima(bestForRoot, rootFirst, rootSecond, candidateEdge);
		}
	}
	return bestForRoot;
}

// Применяет найденные слияния фазы и нужен для наращивания результата MST.
int BoruvkaMstSolver::ApplyPhaseMerges(DisjointSetUnion& disjointSet, MstResult& result, int terminalCount,
	const std::vector<std::optional<UndirectedEdge>>& bestForRoot)
{
	int mergeCount = 0;
	for (int vertexIndex = 0; vertexIndex < terminalCount; ++vertexIndex)
	{
		if (disjointSet.Find(vertexIndex) != vertexIndex)
		{
			continue;
		}
		const std::optional<UndirectedEdge>& chosen = bestForRoot[static_cast<std::size_t>(vertexIndex)];
		if (!chosen.has_value())
		{
			continue;
		}
		const UndirectedEdge edge = *chosen;
		if (!disjointSet.Unite(edge.u, edge.v))
		{
			continue;
		}
		++mergeCount;
		result.total_length += edge.weight;
		result.edges.push_back(edge);
	}
	return mergeCount;
}

// Строит минимальное остовное дерево по Борувке и нужен как базовый эталон для сравнения.
MstResult BoruvkaMstSolver::Build(const std::vector<Point2D>& terminals) const
{
	const int terminalCount = static_cast<int>(terminals.size());
	MstResult result{};
	if (terminalCount <= 1)
	{
		return result;
	}
	DisjointSetUnion disjointSet(static_cast<std::size_t>(terminalCount));
	while (true)
	{
		const std::vector<std::optional<UndirectedEdge>> phaseBest =
			CollectPhaseMinimumEdges(terminals, terminalCount, disjointSet);
		const int mergesThisPhase = ApplyPhaseMerges(disjointSet, result, terminalCount, phaseBest);
		if (mergesThisPhase == 0)
		{
			break;
		}
	}
	return result;
}

} // namespace steiner_lab
