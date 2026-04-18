#pragma once

#include "steiner_lab/boruvka.h"
#include "steiner_lab/euclidean_steiner.h"
#include "steiner_lab/geometry.h"
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace steiner_lab
{

class GraphvizDotExporter
{
public:
	[[nodiscard]] std::string ExportSteinerTree(const std::vector<Point2D>& terminals, const EuclideanSteinerTree& tree,
		std::string_view graphName) const;
	[[nodiscard]] std::string ExportMst(const std::vector<Point2D>& terminals, const MstResult& mst,
		std::string_view graphName) const;

private:
	[[nodiscard]] static bool NearEqualPoints(const Point2D& pointA, const Point2D& pointB);
	[[nodiscard]] static std::string DotNodeIdForPoint(const Point2D& point, const std::vector<Point2D>& terminals,
		const std::vector<Point2D>& steinerPoints);
	static void WriteNeatoPreamble(std::ostringstream& stream, std::string_view graphName);
	static void WriteTerminalVerticesDot(std::ostringstream& stream, const std::vector<Point2D>& terminals);
	static void WriteSteinerVerticesDot(std::ostringstream& stream, const std::vector<Point2D>& steinerPoints,
		const std::vector<Point2D>& terminals, const std::vector<bool>& usedSteinerPoints);
	static void WriteSteinerTreeEdgesDot(std::ostringstream& stream, const EuclideanSteinerTree& tree,
		const std::vector<Point2D>& terminals);
	static void WriteMstEdgesDot(std::ostringstream& stream, const MstResult& mst);
};

} // namespace steiner_lab
