#include "SteinerTree.h"
#include "BoruvkaTree.h"
#include "Point.h"
#include <algorithm>

TreeResult SteinerTree::Build(const std::vector<Point>& terminals)
{
	if (terminals.empty())
	{
		return {};
	}

	if (terminals.size() < 3)
	{
		BoruvkaTree mst;
		return mst.Build(terminals);
	}

	BoruvkaTree mstAlgo;
	TreeResult currentTree = mstAlgo.Build(terminals);

	const size_t initialTerminalsCount = terminals.size();

	// Simple heuristic: try adding Fermat-Torricelli point approximation (centroid) for every triplet
	for (size_t i = 0; i < initialTerminalsCount; ++i)
	{
		for (size_t j = i + 1; j < initialTerminalsCount; ++j)
		{
			for (size_t k = j + 1; k < initialTerminalsCount; ++k)
			{
				std::vector<Point> augmentedPoints = terminals;
				const Point s = Geometry::GetCentroid(
					terminals[i], terminals[j], terminals[k]);

				augmentedPoints.push_back(s);

				TreeResult newTree = mstAlgo.Build(augmentedPoints);

				if (newTree.totalLength < currentTree.totalLength - 1e-9)
				{
					currentTree = std::move(newTree);
				}
			}
		}
	}

	return currentTree;
}