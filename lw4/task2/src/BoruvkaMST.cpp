#include "BoruvkaMST.h"

#include "DisjointSet.h"

#include <limits>
#include <stdexcept>

namespace steiner
{

std::vector<Edge> BoruvkaMST::Build(const Graph& graph) const
{
	const std::size_t n = graph.GetVertexCount();
	std::vector < Edge > result;
	if (n <= 1)
	{
		return result;
	}

	const auto& edges = graph.GetEdges();
	DisjointSet components{ n };
	std::size_t componentsCount = n;
	result.reserve(n - 1);

	static constexpr std::size_t kInvalid = std::numeric_limits<std::size_t>::max();

	while (componentsCount > 1)
	{
		// For every current component store the cheapest outgoing edge index.
		std::vector<std::size_t> cheapest(n, kInvalid);

		for (std::size_t i = 0; i < edges.size(); ++i)
		{
			const Edge& e = edges[i];
			const std::size_t ca = components.Find(e.from);
			const std::size_t cb = components.Find(e.to);
			if (ca == cb)
			{
				continue;
			}
			if (cheapest[ca] == kInvalid || edges[cheapest[ca]].weight > e.weight)
			{
				cheapest[ca] = i;
			}
			if (cheapest[cb] == kInvalid || edges[cheapest[cb]].weight > e.weight)
			{
				cheapest[cb] = i;
			}
		}

		bool progress = false;
		for (std::size_t c = 0; c < n; ++c)
		{
			if (cheapest[c] == kInvalid)
			{
				continue;
			}
			const Edge& e = edges[cheapest[c]];
			if (components.Unite(e.from, e.to))
			{
				result.push_back(e);
				--componentsCount;
				progress = true;
			}
		}

		if (!progress)
		{
			throw std::runtime_error{ "BoruvkaMST::Build: graph is disconnected" };
		}
	}
	return result;
}

} // namespace steiner