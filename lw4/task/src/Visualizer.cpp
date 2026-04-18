#include "Visualizer.h"
#include "Exceptions.h"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace steiner
{
namespace
{

constexpr double kCanvasWidth = 800.0;
constexpr double kCanvasHeight = 600.0;
constexpr double kPadding = 50.0;
constexpr double kSpanEpsilon = 1e-9;
constexpr double kTerminalRadius = 5.0;
constexpr double kSteinerRadius = 4.0;
constexpr double kLabelOffsetX = 6.0;
constexpr double kLabelOffsetY = -6.0;
constexpr int kOutputPrecision = 2;

struct Bounds
{
	double minX = 0.0;
	double maxX = 0.0;
	double minY = 0.0;
	double maxY = 0.0;
	bool initialized = false;
};

// Расширяет границы одной точкой и нужен для корректного охвата всех вершин.
void ExpandBoundsByPoint(Bounds& bounds, const Point2D& point) noexcept
{
	if (!bounds.initialized)
	{
		bounds.minX = point.x;
		bounds.maxX = point.x;
		bounds.minY = point.y;
		bounds.maxY = point.y;
		bounds.initialized = true;
		return;
	}
	bounds.minX = std::min(bounds.minX, point.x);
	bounds.maxX = std::max(bounds.maxX, point.x);
	bounds.minY = std::min(bounds.minY, point.y);
	bounds.maxY = std::max(bounds.maxY, point.y);
}

// Аккумулирует границы по списку точек и нужен для сбора общей области рисования.
void AccumulateBounds(Bounds& bounds, const std::vector<Point2D>& points) noexcept
{
	for (const Point2D& point : points)
	{
		ExpandBoundsByPoint(bounds, point);
	}
}

// Проецирует точку мира на экран и нужен для единого масштабирования всех SVG-слоёв.
class Projector
{
public:
	explicit Projector(const Bounds& bounds) noexcept
		: m_bounds(bounds)
	{
		const double spanX = (bounds.maxX - bounds.minX) + kSpanEpsilon;
		const double spanY = (bounds.maxY - bounds.minY) + kSpanEpsilon;
		const double scaleX = (kCanvasWidth - 2.0 * kPadding) / spanX;
		const double scaleY = (kCanvasHeight - 2.0 * kPadding) / spanY;
		m_scale = std::min(scaleX, scaleY);
	}

	std::pair<double, double> operator()(const Point2D& point) const noexcept
	{
		const double screenX = kPadding + (point.x - m_bounds.minX) * m_scale;
		// Flip Y so that mathematical Y grows upward on screen.
		const double screenY = kCanvasHeight - (kPadding + (point.y - m_bounds.minY) * m_scale);
		return { screenX, screenY };
	}

private:
	Bounds m_bounds{};
	double m_scale = 1.0;
};

// Записывает SVG-линию по двум точкам и нужен для рисования рёбер любого из деревьев.
void WriteSvgLine(std::ostream& stream, const std::pair<double, double>& from, const std::pair<double, double>& to,
	const char* color)
{
	stream << "<line x1='" << from.first << "' y1='" << from.second << "' x2='" << to.first << "' y2='" << to.second
		<< "' stroke='" << color << "' stroke-width='2' />\n";
}

// Записывает SVG-круг и нужен для отрисовки терминалов и Штейнер-вершин.
void WriteSvgCircle(std::ostream& stream, double centerX, double centerY, double radius, const char* color)
{
	stream << "<circle cx='" << centerX << "' cy='" << centerY << "' r='" << radius << "' fill='" << color << "' />\n";
}

// Записывает SVG-подпись рядом с точкой и нужен для человекочитаемых меток вершин.
void WriteSvgLabel(std::ostream& stream, double centerX, double centerY, const std::string& text)
{
	if (text.empty())
	{
		return;
	}
	stream << "<text x='" << centerX + kLabelOffsetX << "' y='" << centerY + kLabelOffsetY
		<< "' font-family='sans-serif' font-size='12' fill='black'>" << text << "</text>\n";
}

// Рисует рёбра MST и нужен как слой для картинки остовного дерева.
void WriteMstEdges(std::ostream& stream, const std::vector<Point2D>& terminals, const MstResult& mst,
	const Projector& project, const char* color)
{
	for (const UndirectedEdge& edge : mst.edges)
	{
		const auto from = project(terminals[static_cast<std::size_t>(edge.u)]);
		const auto to = project(terminals[static_cast<std::size_t>(edge.v)]);
		WriteSvgLine(stream, from, to, color);
	}
}

// Рисует рёбра дерева Штейнера и нужен как слой для картинки Steiner tree.
void WriteSteinerEdges(std::ostream& stream, const SteinerTreeResult& tree, const Projector& project, const char* color)
{
	for (const std::pair<Point2D, Point2D>& segment : tree.segments)
	{
		const auto from = project(segment.first);
		const auto to = project(segment.second);
		WriteSvgLine(stream, from, to, color);
	}
}

// Рисует терминалы с подписями и нужен как общий слой для обеих картинок.
void WriteTerminalNodes(std::ostream& stream, const std::vector<Point2D>& terminals, const Projector& project,
	const char* color)
{
	for (std::size_t terminalIndex = 0; terminalIndex < terminals.size(); ++terminalIndex)
	{
		const auto [screenX, screenY] = project(terminals[terminalIndex]);
		WriteSvgCircle(stream, screenX, screenY, kTerminalRadius, color);
		WriteSvgLabel(stream, screenX, screenY, "T" + std::to_string(terminalIndex));
	}
}

// Рисует Штейнер-вершины с подписями и нужен для визуального отличия от терминалов.
void WriteSteinerNodes(std::ostream& stream, const std::vector<Point2D>& steinerPoints, const Projector& project,
	const char* color)
{
	for (std::size_t steinerIndex = 0; steinerIndex < steinerPoints.size(); ++steinerIndex)
	{
		const auto [screenX, screenY] = project(steinerPoints[steinerIndex]);
		WriteSvgCircle(stream, screenX, screenY, kSteinerRadius, color);
		WriteSvgLabel(stream, screenX, screenY, "S" + std::to_string(steinerIndex));
	}
}

// Пишет открывающий тег SVG и нужен для унифицированного старта обеих картинок.
void OpenSvg(std::ostream& stream)
{
	stream << "<svg width='" << kCanvasWidth << "' height='" << kCanvasHeight << "' style='border:1px solid black'>\n";
}

// Пишет закрывающий тег SVG и нужен для унифицированного завершения обеих картинок.
void CloseSvg(std::ostream& stream)
{
	stream << "</svg>\n";
}

// Рисует секцию MST и нужен для изоляции верстки первой картинки.
void WriteMstSection(std::ostream& stream, const std::vector<Point2D>& terminals, const MstResult& mst,
	const Projector& project)
{
	stream << "<h2>Minimum Spanning Tree (Boruvka)</h2>\n";
	stream << "<p>Length: " << mst.total_length << "</p>\n";
	OpenSvg(stream);
	WriteMstEdges(stream, terminals, mst, project, "blue");
	WriteTerminalNodes(stream, terminals, project, "blue");
	CloseSvg(stream);
}

// Рисует секцию Steiner tree и нужен для изоляции верстки второй картинки.
void WriteSteinerSection(std::ostream& stream, const std::vector<Point2D>& terminals,
	const SteinerTreeResult& steinerResult, const Projector& project)
{
	stream << "<h2>Steiner Tree (Exact)</h2>\n";
	stream << "<p>Length: " << steinerResult.total_length << "</p>\n";
	OpenSvg(stream);
	WriteSteinerEdges(stream, steinerResult, project, "red");
	WriteTerminalNodes(stream, terminals, project, "black");
	WriteSteinerNodes(stream, steinerResult.steiner_points, project, "green");
	CloseSvg(stream);
}

// Пишет HTML-шапку документа и нужен для корректного рендеринга в браузере.
void WriteHtmlHead(std::ostream& stream)
{
	stream << "<!DOCTYPE html>\n";
	stream << "<html><head><meta charset='utf-8'><title>Steiner vs MST</title></head><body>\n";
}

// Пишет HTML-подвал документа и нужен для корректного закрытия тегов.
void WriteHtmlFoot(std::ostream& stream)
{
	stream << "</body></html>\n";
}

// Считает общие границы двух деревьев и нужен для согласованного масштаба двух SVG.
Bounds ComputeOverallBounds(const std::vector<Point2D>& terminals, const SteinerTreeResult& steinerResult)
{
	Bounds bounds{};
	AccumulateBounds(bounds, terminals);
	AccumulateBounds(bounds, steinerResult.steiner_points);
	if (!bounds.initialized)
	{
		bounds = Bounds{ 0.0, 1.0, 0.0, 1.0, true };
	}
	return bounds;
}

} // namespace

// Сохраняет сравнительную визуализацию в HTML/SVG и нужен для наглядного отчёта пользователю.
void Visualizer::SaveToHtml(const std::vector<Point2D>& terminals, const MstResult& mst,
	const SteinerTreeResult& steinerResult, const std::string& filename)
{
	std::ofstream out{ filename };
	if (!out.is_open())
	{
		throw IoError("не удалось открыть файл для записи: " + filename);
	}

	const Bounds bounds = ComputeOverallBounds(terminals, steinerResult);
	const Projector project{ bounds };

	out << std::fixed << std::setprecision(kOutputPrecision);
	WriteHtmlHead(out);
	WriteMstSection(out, terminals, mst, project);
	WriteSteinerSection(out, terminals, steinerResult, project);
	WriteHtmlFoot(out);

	if (!out.good())
	{
		throw IoError("ошибка записи в файл: " + filename);
	}
}

} // namespace steiner