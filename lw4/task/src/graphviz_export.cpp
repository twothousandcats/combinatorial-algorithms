#include "steiner_lab/graphviz_export.h"
#include "steiner_lab/constants.h"

#include <cmath>
#include <optional>
#include <sstream>
#include <string>

namespace steiner_lab
{

using graphviz_constants::kCoordinateMatchEpsilon;
using graphviz_constants::kDotNodeCoordinateScale;

namespace
{
constexpr double kSteinerVisualShift = 1.0;

// Сравнивает точки с допуском и нужна для устойчивого поиска соответствующих узлов в DOT.
bool PointsNearlyEqual(const Point2D& pointA, const Point2D& pointB)
{
	if (std::fabs(pointA.x - pointB.x) > kCoordinateMatchEpsilon)
	{
		return false;
	}
	if (std::fabs(pointA.y - pointB.y) > kCoordinateMatchEpsilon)
	{
		return false;
	}
	return true;
}

// Пытается найти id узла по списку точек и нужна для корректной привязки концов сегментов.
std::optional<std::string> TryDotNodeIdFromPointList(const Point2D& target, const std::vector<Point2D>& points,
	char labelPrefix)
{
	for (std::size_t pointIndex = 0; pointIndex < points.size(); ++pointIndex)
	{
		if (PointsNearlyEqual(target, points[pointIndex]))
		{
			return std::string(1, labelPrefix) + std::to_string(pointIndex);
		}
	}
	return std::nullopt;
}

bool PointOverlapsAny(const Point2D& target, const std::vector<Point2D>& points)
{
	for (const Point2D& point : points)
	{
		if (PointsNearlyEqual(target, point))
		{
			return true;
		}
	}
	return false;
}

void MarkSteinerPointIfAddressedAsSteiner(const Point2D& endpoint, const std::vector<Point2D>& terminals,
	const std::vector<Point2D>& steinerPoints, std::vector<bool>& usedSteinerPoints)
{
	std::string endpointNodeId;
	if (const std::optional<std::string> terminalNodeId = TryDotNodeIdFromPointList(endpoint, terminals, 'T'))
	{
		endpointNodeId = *terminalNodeId;
	}
	else if (const std::optional<std::string> steinerNodeId = TryDotNodeIdFromPointList(endpoint, steinerPoints, 'S'))
	{
		endpointNodeId = *steinerNodeId;
	}
	else
	{
		const long long scaledX = static_cast<long long>(std::llround(endpoint.x * kDotNodeCoordinateScale));
		const long long scaledY = static_cast<long long>(std::llround(endpoint.y * kDotNodeCoordinateScale));
		endpointNodeId = std::string("U_") + std::to_string(scaledX) + "_" + std::to_string(scaledY);
	}
	if (endpointNodeId.size() < 2 || endpointNodeId.front() != 'S')
	{
		return;
	}
	const std::size_t steinerIndex = static_cast<std::size_t>(std::stoull(endpointNodeId.substr(1)));
	if (steinerIndex < usedSteinerPoints.size())
	{
		usedSteinerPoints[steinerIndex] = true;
	}
}

std::vector<bool> CollectUsedSteinerPoints(const EuclideanSteinerTree& tree, const std::vector<Point2D>& terminals)
{
	std::vector<bool> usedSteinerPoints(tree.steiner_points.size(), false);
	for (const auto& segment : tree.segments)
	{
		std::string idA;
		std::string idB;
		if (const std::optional<std::string> terminalA = TryDotNodeIdFromPointList(segment.first, terminals, 'T'))
		{
			idA = *terminalA;
		}
		else if (const std::optional<std::string> steinerA = TryDotNodeIdFromPointList(segment.first, tree.steiner_points, 'S'))
		{
			idA = *steinerA;
		}
		else
		{
			const long long scaledX = static_cast<long long>(std::llround(segment.first.x * kDotNodeCoordinateScale));
			const long long scaledY = static_cast<long long>(std::llround(segment.first.y * kDotNodeCoordinateScale));
			idA = std::string("U_") + std::to_string(scaledX) + "_" + std::to_string(scaledY);
		}
		if (const std::optional<std::string> terminalB = TryDotNodeIdFromPointList(segment.second, terminals, 'T'))
		{
			idB = *terminalB;
		}
		else if (const std::optional<std::string> steinerB = TryDotNodeIdFromPointList(segment.second, tree.steiner_points, 'S'))
		{
			idB = *steinerB;
		}
		else
		{
			const long long scaledX = static_cast<long long>(std::llround(segment.second.x * kDotNodeCoordinateScale));
			const long long scaledY = static_cast<long long>(std::llround(segment.second.y * kDotNodeCoordinateScale));
			idB = std::string("U_") + std::to_string(scaledX) + "_" + std::to_string(scaledY);
		}
		if (idA == idB)
		{
			continue;
		}
		MarkSteinerPointIfAddressedAsSteiner(segment.first, terminals, tree.steiner_points, usedSteinerPoints);
		MarkSteinerPointIfAddressedAsSteiner(segment.second, terminals, tree.steiner_points, usedSteinerPoints);
	}
	return usedSteinerPoints;
}
} // namespace

// Делегирует сравнение точек с допуском и нужен как публичный helper экспортера.
bool GraphvizDotExporter::NearEqualPoints(const Point2D& pointA, const Point2D& pointB)
{
	return PointsNearlyEqual(pointA, pointB);
}

// Вычисляет id DOT-узла по координате и нужен для сборки ребер в экспортируемом графе.
std::string GraphvizDotExporter::DotNodeIdForPoint(const Point2D& point, const std::vector<Point2D>& terminals,
	const std::vector<Point2D>& steinerPoints)
{
	if (const std::optional<std::string> terminalNodeId = TryDotNodeIdFromPointList(point, terminals, 'T'))
	{
		return *terminalNodeId;
	}
	if (const std::optional<std::string> steinerNodeId = TryDotNodeIdFromPointList(point, steinerPoints, 'S'))
	{
		return *steinerNodeId;
	}
	const long long scaledX = static_cast<long long>(std::llround(point.x * kDotNodeCoordinateScale));
	const long long scaledY = static_cast<long long>(std::llround(point.y * kDotNodeCoordinateScale));
	return std::string("U_") + std::to_string(scaledX) + "_" + std::to_string(scaledY);
}

// Записывает заголовок графа neato и нужен для корректной визуализации фиксированных позиций.
void GraphvizDotExporter::WriteNeatoPreamble(std::ostringstream& stream, std::string_view graphName)
{
	stream << "graph " << graphName << " {\n";
	stream << "  layout=neato;\n";
	stream << "  overlap=false;\n";
	stream << "  splines=true;\n";
	stream << "  // Hint: use neato engine on graphviz viewers for fixed pos= coordinates.\n";
}

// Добавляет терминалы в DOT и нужна для отображения входных точек.
void GraphvizDotExporter::WriteTerminalVerticesDot(std::ostringstream& stream, const std::vector<Point2D>& terminals)
{
	for (std::size_t terminalIndex = 0; terminalIndex < terminals.size(); ++terminalIndex)
	{
		stream << "  T" << terminalIndex << " [shape=box, label=\"T" << terminalIndex << "\", pos=\""
			   << terminals[terminalIndex].x << "," << terminals[terminalIndex].y << "!\"];\n";
	}
}

// Добавляет точки Штейнера в DOT и нужна для визуального разделения типов вершин.
void GraphvizDotExporter::WriteSteinerVerticesDot(std::ostringstream& stream, const std::vector<Point2D>& steinerPoints,
	const std::vector<Point2D>& terminals, const std::vector<bool>& usedSteinerPoints)
{
	for (std::size_t steinerIndex = 0; steinerIndex < steinerPoints.size(); ++steinerIndex)
	{
		if (steinerIndex >= usedSteinerPoints.size() || !usedSteinerPoints[steinerIndex])
		{
			continue;
		}
		const Point2D& steiner = steinerPoints[steinerIndex];
		double drawX = steiner.x;
		double drawY = steiner.y;
		if (PointOverlapsAny(steiner, terminals))
		{
			const double offset = kSteinerVisualShift * static_cast<double>(steinerIndex + 1);
			drawX += offset;
			drawY += offset;
		}
		stream << "  S" << steinerIndex << " [shape=diamond, label=\"S" << steinerIndex << "\", pos=\"" << drawX << ","
			   << drawY << "!\"];\n";
	}
}

// Добавляет ребра дерева Штейнера в DOT и нужна для отображения результата оптимизации.
void GraphvizDotExporter::WriteSteinerTreeEdgesDot(std::ostringstream& stream, const EuclideanSteinerTree& tree,
	const std::vector<Point2D>& terminals)
{
	std::size_t edgeIndex = 0;
	for (const auto& segment : tree.segments)
	{
		std::string idA = DotNodeIdForPoint(segment.first, terminals, tree.steiner_points);
		std::string idB = DotNodeIdForPoint(segment.second, terminals, tree.steiner_points);
		if (idA == idB)
		{
			// Segment endpoints collapse to one logical node in DOT mapping.
			// Skip such degenerate edges to avoid tiny "empty" T-S links in the picture.
			continue;
		}
		stream << "  \"" << idA << "\" -- \"" << idB << "\" [label=\"e" << edgeIndex++ << "\"];\n";
	}
}

// Добавляет ребра остовного дерева в DOT и нужна для визуального сравнения с Штейнером.
void GraphvizDotExporter::WriteMstEdgesDot(std::ostringstream& stream, const MstResult& mst)
{
	std::size_t edgeIndex = 0;
	for (const UndirectedEdge& edge : mst.edges)
	{
		stream << "  T" << edge.u << " -- T" << edge.v << " [label=\"e" << edgeIndex++ << "\", weight=\"" << edge.weight
			   << "\"];\n";
	}
}

// Экспортирует полное дерево Штейнера в строку DOT и нужен для последующей визуализации.
std::string GraphvizDotExporter::ExportSteinerTree(const std::vector<Point2D>& terminals, const EuclideanSteinerTree& tree,
	std::string_view graphName) const
{
	std::ostringstream stream;
	WriteNeatoPreamble(stream, graphName);
	WriteTerminalVerticesDot(stream, terminals);
	const std::vector<bool> usedSteinerPoints = CollectUsedSteinerPoints(tree, terminals);
	WriteSteinerVerticesDot(stream, tree.steiner_points, terminals, usedSteinerPoints);
	WriteSteinerTreeEdgesDot(stream, tree, terminals);
	stream << "}\n";
	return stream.str();
}

// Экспортирует остовное дерево в строку DOT и нужен для графического сравнения алгоритмов.
std::string GraphvizDotExporter::ExportMst(const std::vector<Point2D>& terminals, const MstResult& mst,
	std::string_view graphName) const
{
	std::ostringstream stream;
	WriteNeatoPreamble(stream, graphName);
	WriteTerminalVerticesDot(stream, terminals);
	WriteMstEdgesDot(stream, mst);
	stream << "}\n";
	return stream.str();
}

} // namespace steiner_lab
