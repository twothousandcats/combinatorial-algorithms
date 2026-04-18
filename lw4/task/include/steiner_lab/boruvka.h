#pragma once

#include "steiner_lab/geometry.h"
#include <cstddef>
#include <optional>
#include <vector>

namespace steiner_lab
{

struct UndirectedEdge
{
	int u = 0;
	int v = 0;
	double weight = 0.0;
};

struct MstResult
{
	double total_length = 0.0;
	std::vector<UndirectedEdge> edges{};
};

class BoruvkaMstSolver
{
public:
	[[nodiscard]] MstResult Build(const std::vector<Point2D>& terminals) const;

private:
	class DisjointSetUnion
	{
	public:
		explicit DisjointSetUnion(std::size_t elementCount);
		int Find(int elementIndex);
		bool Unite(int firstIndex, int secondIndex);

	private:
		void AttachChildToParent(int childRoot, int parentRoot);
		void BumpParentRankIfTied(int parentRoot, bool siblingRanksWereEqual);

		std::vector<int> m_parent;
		std::vector<int> m_rank;
	};

	[[nodiscard]] static double TerminalEdgeWeight(const std::vector<Point2D>& terminals, int firstIndex, int secondIndex);
	static void ConsiderEdgeForComponentMinima(std::vector<std::optional<UndirectedEdge>>& bestForRoot, int rootFirst,
		int rootSecond, const UndirectedEdge& edge);
	[[nodiscard]] static std::vector<std::optional<UndirectedEdge>> CollectPhaseMinimumEdges(
		const std::vector<Point2D>& terminals, int terminalCount, DisjointSetUnion& disjointSet);
	[[nodiscard]] static int ApplyPhaseMerges(DisjointSetUnion& disjointSet, MstResult& result, int terminalCount,
		const std::vector<std::optional<UndirectedEdge>>& bestForRoot);
};

} // namespace steiner_lab
