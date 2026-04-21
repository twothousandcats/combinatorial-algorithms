#include "Visualizer.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace steiner
{
namespace
{

constexpr double kCanvasWidth = 800.0;
constexpr double kCanvasHeight = 600.0;
constexpr double kPadding = 50.0;
constexpr double kSpanEpsilon = 1e-9;

struct Bounds
{
	double minX = 1e9;
	double maxX = -1e9;
	double minY = 1e9;
	double maxY = -1e9;
};

void AccumulateBounds(Bounds& bounds, const std::vector<Point>& points) noexcept
{
	for (const auto& p : points)
	{
		bounds.minX = std::min(bounds.minX, p.x);
		bounds.maxX = std::max(bounds.maxX, p.x);
		bounds.minY = std::min(bounds.minY, p.y);
		bounds.maxY = std::max(bounds.maxY, p.y);
	}
}

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

	std::pair<double, double> operator()(const Point& p) const noexcept
	{
		const double sx = kPadding + (p.x - m_bounds.minX) * m_scale;
		// Flip Y so that mathematical Y grows upward on screen.
		const double sy = kCanvasHeight - (kPadding + (p.y - m_bounds.minY) * m_scale);
		return { sx, sy };
	}

private:
	Bounds m_bounds{};
	double m_scale = 1.0;
};

double SumWeights(const std::vector<Edge>& edges) noexcept
{
	double sum = 0.0;
	for (const auto& e : edges)
	{
		sum += e.weight;
	}
	return sum;
}

void WriteEdges(std::ostream& out,
	const std::vector<Point>& vertices,
	const std::vector<Edge>& edges,
	const Projector& project,
	const char* color)
{
	for (const auto& e : edges)
	{
		const auto [x1, y1] = project(vertices[e.from]);
		const auto [x2, y2] = project(vertices[e.to]);
		out << "<line x1='" << x1 << "' y1='" << y1
			<< "' x2='" << x2 << "' y2='" << y2
			<< "' stroke='" << color << "' stroke-width='2' />\n";
	}
}

void WriteCircle(std::ostream& out, double cx, double cy, double radius, const char* color)
{
	out << "<circle cx='" << cx << "' cy='" << cy
		<< "' r='" << radius << "' fill='" << color << "' />\n";
}

void WriteLabel(std::ostream& out, double cx, double cy, const std::string& text)
{
	if (text.empty())
	{
		return;
	}
	out << "<text x='" << cx + 6.0 << "' y='" << cy - 6.0
		<< "' font-family='sans-serif' font-size='12' fill='black'>"
		<< text << "</text>\n";
}

} // namespace

void Visualizer::SaveToHtml(
	const std::vector<Point>& terminals,
	const std::vector<Edge>& mstEdges,
	const SteinerTreeResult& steinerResult,
	const std::string& filename)
{
	std::ofstream out{ filename };
	if (!out.is_open())
	{
		throw std::runtime_error{ "Visualizer: cannot open '" + filename + "' for writing" };
	}

	Bounds bounds{};
	AccumulateBounds(bounds, terminals);
	AccumulateBounds(bounds, steinerResult.vertices);
	const Projector project{ bounds };

	out << std::fixed << std::setprecision(2);
	out << "<!DOCTYPE html>\n";
	out << "<html><head><meta charset='utf-8'><title>Steiner vs MST</title></head><body>\n";

	// MST picture.
	out << "<h2>MST</h2>\n";
	out << "<p>Length: " << SumWeights(mstEdges) << "</p>\n";
	out << "<svg width='" << kCanvasWidth << "' height='" << kCanvasHeight
		<< "' style='border:1px solid black'>\n";
	WriteEdges(out, terminals, mstEdges, project, "blue");
	for (const auto& p : terminals)
	{
		const auto [x, y] = project(p);
		WriteCircle(out, x, y, 4.0, "blue");
		WriteLabel(out, x, y, p.name);
	}
	out << "</svg>\n";

	// Steiner picture
	out << "<h2>Steiner Tree</h2>\n";
	out << "<p>Length: " << steinerResult.totalLength << "</p>\n";
	out << "<svg width='" << kCanvasWidth << "' height='" << kCanvasHeight
		<< "' style='border:1px solid black'>\n";
	WriteEdges(out, steinerResult.vertices, steinerResult.edges, project, "red");

	const std::size_t terminalCount = terminals.size();
	for (std::size_t i = 0; i < terminalCount; ++i)
	{
		const auto [x, y] = project(steinerResult.vertices[i]);
		WriteCircle(out, x, y, 5.0, "black");
		WriteLabel(out, x, y, steinerResult.vertices[i].name);
	}
	for (std::size_t i = terminalCount; i < steinerResult.vertices.size(); ++i)
	{
		const auto [x, y] = project(steinerResult.vertices[i]);
		WriteCircle(out, x, y, 4.0, "green");
		WriteLabel(out, x, y, steinerResult.vertices[i].name);
	}
	out << "</svg>\n";

	out << "</body></html>\n";
}

} // namespace steiner