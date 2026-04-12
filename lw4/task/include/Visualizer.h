#pragma once
#include "Graph.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

class Visualizer
{
public:
	static void SaveToHtml(const Graph& mst, const Graph& steiner, const std::string& filename)
	{
		std::ofstream out(filename);
		if (!out.is_open())
			return;

		// Determine bounds
		double minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9;
		auto updateBounds = [&](const Graph& g) {
			for (const auto& p : g.nodes)
			{
				minX = std::min(minX, p.x);
				maxX = std::max(maxX, p.x);
				minY = std::min(minY, p.y);
				maxY = std::max(maxY, p.y);
			}
		};
		updateBounds(mst);
		updateBounds(steiner);

		double padding = 50.0;
		double width = 800;
		double height = 600;

		double scaleX = (width - 2 * padding) / (maxX - minX + 1e-9);
		double scaleY = (height - 2 * padding) / (maxY - minY + 1e-9);
		double scale = std::min(scaleX, scaleY);

		auto toScreen = [&](const geometry::Point& p) {
			double sx = padding + (p.x - minX) * scale;
			double sy = height - (padding + (p.y - minY) * scale); // Flip Y
			return std::make_pair(sx, sy);
		};

		out << "<!DOCTYPE html>\n<html><body>\n";
		out << "<h2>Minimum Spanning Tree (Boruvka)</h2>\n";
		out << "<p>Length: " << std::fixed << std::setprecision(2) << mst.TotalWeight() << "</p>\n";
		out << "<svg width='" << width << "' height='" << height << "' style='border:1px solid black'>\n";

		// Draw MST Edges
		for (const auto& e : mst.edges)
		{
			auto [x1, y1] = toScreen(mst.nodes[e.u]);
			auto [x2, y2] = toScreen(mst.nodes[e.v]);
			out << "<line x1='" << x1 << "' y1='" << y1 << "' x2='" << x2 << "' y2='" << y2
				<< "' stroke='blue' stroke-width='2' />\n";
		}
		// Draw MST Nodes
		for (const auto& p : mst.nodes)
		{
			auto [x, y] = toScreen(p);
			out << "<circle cx='" << x << "' cy='" << y << "' r='4' fill='blue' />\n";
		}
		out << "</svg>\n";

		out << "<h2>Steiner Tree (Heuristic)</h2>\n";
		out << "<p>Length: " << std::fixed << std::setprecision(2) << steiner.TotalWeight() << "</p>\n";
		out << "<svg width='" << width << "' height='" << height << "' style='border:1px solid black'>\n";

		// Draw Steiner Edges
		for (const auto& e : steiner.edges)
		{
			auto [x1, y1] = toScreen(steiner.nodes[e.u]);
			auto [x2, y2] = toScreen(steiner.nodes[e.v]);
			out << "<line x1='" << x1 << "' y1='" << y1 << "' x2='" << x2 << "' y2='" << y2
				<< "' stroke='red' stroke-width='2' />\n";
		}

		// Draw Original Terminals
		for (size_t i = 0; i < mst.nodes.size(); ++i)
		{
			// Assuming first N are terminals
			// Note: In our heuristic, terminals are the first nodes added.
			// But Steiner graph might have reordered nodes if we rebuilt MST from scratch?
			// In MstBoruvka::Compute, we copy nodes. In Steiner heuristic, we append.
			// So indices 0..OriginalN-1 are terminals.
		}

		// To distinguish, we need to know how many original terminals there were.
		// Let's assume the user passes original count or we detect by color.
		// For simplicity, all nodes in Steiner graph are drawn, but Steiner points are different color.
		// We don't track which is which in Graph struct easily.
		// Let's assume Steiner points were added last.

		int originalCount = static_cast<int>(mst.nodes.size());

		// Draw Terminals (Black)
		for (int i = 0; i < originalCount; ++i)
		{
			auto [x, y] = toScreen(steiner.nodes[i]);
			out << "<circle cx='" << x << "' cy='" << y << "' r='5' fill='black' />\n";
		}

		// Draw Steiner Points (Green)
		for (size_t i = originalCount; i < steiner.nodes.size(); ++i)
		{
			auto [x, y] = toScreen(steiner.nodes[i]);
			out << "<circle cx='" << x << "' cy='" << y << "' r='4' fill='green' />\n";
		}

		out << "</svg>\n";
		out << "</body></html>\n";
	}
};