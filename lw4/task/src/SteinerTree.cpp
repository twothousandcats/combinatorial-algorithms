#include "SteinerTree.h"
#include "BoruvkaTree.h"
#include "Point.h"
#include <algorithm>

TreeResult SteinerTree::Build(const std::vector<Point>& points)
{
	if (points.empty())
	{
		return {};
	}

	if (points.size() < 3)
	{
		BoruvkaTree mst;
		return mst.Build(points);
	}

	BoruvkaTree mstAlgo;
	TreeResult currentTree = mstAlgo.Build(points);

	const size_t initialTerminalsCount = points.size();

	// adding Fermat-Torricelli point for every triplet
	for (size_t i = 0; i < initialTerminalsCount; ++i)
	{
		for (size_t j = i + 1; j < initialTerminalsCount; ++j)
		{
			for (size_t k = j + 1; k < initialTerminalsCount; ++k)
			{
				std::vector<Point> augmentedPoints = points;
				const Point s = Geometry::GetFermatPoint(
					points[i], points[j], points[k]
					);

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