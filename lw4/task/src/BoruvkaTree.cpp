#include "BoruvkaTree.h"
#include "Point.h"
#include <numeric>

TreeResult BoruvkaTree::Build(const std::vector<Point>& points)
{
	const auto n = static_cast<int>(points.size());
	if (n == 0)
	{
		return {};
	}

	std::vector<int> parent(n);
	std::iota(parent.begin(), parent.end(), 0);

	auto find = [&](int v) {
		while (v != parent[v])
		{
			parent[v] = parent[parent[v]];
			v = parent[v];
		}
		return v;
	};

	auto unite = [&](int a, int b) -> bool {
		a = find(a);
		b = find(b);
		if (a != b)
		{
			parent[a] = b;
			return true;
		}
		return false;
	};

	TreeResult result;
	int components = n;
	struct FullEdge
	{
		int u;
		int v;
		double w;
	};

	std::vector<FullEdge> allEdges;
	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			allEdges.push_back({ i, j, Geometry::Distance(points[i], points[j]) });
		}
	}

	while (components > 1)
	{
		std::vector<int> closestEdgeIdx(n, -1);

		for (size_t i = 0; i < allEdges.size(); ++i)
		{
			const auto& e = allEdges[i];
			const int setU = find(e.u);
			const int setV = find(e.v);

			if (setU == setV)
				continue;

			if (closestEdgeIdx[setU] == -1 ||
				allEdges[closestEdgeIdx[setU]].w > e.w)
			{
				closestEdgeIdx[setU] = static_cast<int>(i);
			}
			if (closestEdgeIdx[setV] == -1 ||
				allEdges[closestEdgeIdx[setV]].w > e.w)
			{
				closestEdgeIdx[setV] = static_cast<int>(i);
			}
		}

		bool merged = false;
		for (int i = 0; i < n; ++i)
		{
			if (closestEdgeIdx[i] != -1)
			{
				const auto& [u, v, w] = allEdges[closestEdgeIdx[i]];
				if (unite(u, v))
				{
					result.edges.push_back({ u, v, w });
					result.totalLength += w;
					--components;
					merged = true;
				}
			}
		}

		if (!merged)
			break;
	}

	return result;
}