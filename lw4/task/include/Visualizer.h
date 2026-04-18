#pragma once

#include "Boruvka.h"
#include "Geometry.h"
#include "SteinerSolver.h"

#include <string>
#include <vector>

namespace steiner
{

class Visualizer
{
public:
	static void SaveToHtml(const std::vector<Point2D>& terminals, const MstResult& mst,
		const SteinerTreeResult& steinerResult, const std::string& filename);
};

} // namespace steiner