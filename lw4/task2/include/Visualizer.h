#pragma once

#include "Edge.h"
#include "Point.h"
#include "SteinerTree.h"

#include <string>
#include <vector>

namespace steiner
{
class Visualizer
{
public:
	static void SaveToHtml(
		const std::vector<Point>& terminals,
		const std::vector<Edge>& mstEdges,
		const SteinerTreeResult& steinerResult,
		const std::string& filename);
};

} // namespace steiner