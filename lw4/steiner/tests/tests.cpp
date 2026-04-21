#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "BoruvkaMST.h"
#include "DisjointSet.h"
#include "Geometry.h"
#include "Graph.h"
#include "InputReader.h"
#include "Point.h"
#include "SteinerTree.h"
#include "Visualizer.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

using Catch::Approx;
using namespace steiner;

TEST_CASE("DisjointSet unites and finds components", "[disjoint]")
{
	DisjointSet ds{ 5 };
	REQUIRE(ds.Find(0) != ds.Find(1));
	REQUIRE(ds.Unite(0, 1));
	REQUIRE(ds.Find(0) == ds.Find(1));
	REQUIRE_FALSE(ds.Unite(0, 1));
	REQUIRE(ds.Unite(1, 2));
	REQUIRE(ds.Find(0) == ds.Find(2));
	REQUIRE(ds.Find(3) != ds.Find(0));
}

TEST_CASE("Distance returns Euclidean length", "[point]")
{
	const Point a{ "A", 0.0, 0.0 };
	const Point b{ "B", 3.0, 4.0 };
	REQUIRE(Distance(a, b) == Approx(5.0));
}

TEST_CASE("Complete graph has n*(n-1)/2 edges", "[graph]")
{
	const std::vector<Point> points{
		{ "A", 0.0, 0.0 },
		{ "B", 1.0, 0.0 },
		{ "C", 0.0, 1.0 }
	};
	const auto graph = Graph::BuildComplete(points);
	REQUIRE(graph.GetVertexCount() == 3);
	REQUIRE(graph.GetEdges().size() == 3);
}

TEST_CASE("BoruvkaMST on a unit square", "[boruvka]")
{
	const std::vector<Point> points{
		{ "A", 0.0, 0.0 },
		{ "B", 1.0, 0.0 },
		{ "C", 0.0, 1.0 },
		{ "D", 1.0, 1.0 }
	};
	const auto graph = Graph::BuildComplete(points);
	const BoruvkaMST builder;
	const auto mst = builder.Build(graph);
	REQUIRE(mst.size() == 3);
	REQUIRE(TotalWeight(mst) == Approx(3.0));
}

TEST_CASE("BoruvkaMST on a single vertex is empty", "[boruvka]")
{
	const std::vector<Point> points{ { "A", 0.0, 0.0 } };
	const auto graph = Graph::BuildComplete(points);
	const BoruvkaMST builder;
	REQUIRE(builder.Build(graph).empty());
}

TEST_CASE("Fermat point of an equilateral triangle is its centroid", "[geometry]")
{
	const Point a{ "A", 0.0, 0.0 };
	const Point b{ "B", 1.0, 0.0 };
	const Point c{ "C", 0.5, std::sqrt(3.0) / 2.0 };
	const Point fermat = ComputeFermatPoint(a, b, c);
	REQUIRE(fermat.x == Approx((a.x + b.x + c.x) / 3.0).margin(1e-6));
	REQUIRE(fermat.y == Approx((a.y + b.y + c.y) / 3.0).margin(1e-6));
}

TEST_CASE("Fermat point collapses to an obtuse vertex", "[geometry]")
{
	const Point a{ "A", -1.0, 0.0 };
	const Point b{ "B", 0.0, 0.0 };
	const Point c{ "C", 1.0, 0.1 };
	const Point fermat = ComputeFermatPoint(a, b, c);
	REQUIRE(fermat.x == Approx(b.x));
	REQUIRE(fermat.y == Approx(b.y));
}

TEST_CASE("Steiner tree on equilateral triangle beats MST", "[steiner]")
{
	const double side = 1.0;
	const std::vector<Point> terminals{
		{ "A", 0.0, 0.0 },
		{ "B", side, 0.0 },
		{ "C", side / 2.0, side * std::sqrt(3.0) / 2.0 }
	};
	const auto graph = Graph::BuildComplete(terminals);
	const BoruvkaMST mstBuilder;
	const double mstLength = TotalWeight(mstBuilder.Build(graph));
	REQUIRE(mstLength == Approx(2.0 * side));

	const SteinerTreeBuilder steinerBuilder;
	const auto result = steinerBuilder.Build(terminals);
	// Optimal Steiner length for an equilateral triangle with side s is s * sqrt(3).
	REQUIRE(result.totalLength == Approx(side * std::sqrt(3.0)).margin(1e-4));
	REQUIRE(result.totalLength < mstLength);
	REQUIRE(result.vertices.size() == terminals.size() + 1);
}

TEST_CASE("Steiner tree on collinear points matches MST", "[steiner]")
{
	const std::vector<Point> terminals{
		{ "A", 0.0, 0.0 },
		{ "B", 1.0, 0.0 },
		{ "C", 2.0, 0.0 },
		{ "D", 3.0, 0.0 }
	};
	const auto graph = Graph::BuildComplete(terminals);
	const BoruvkaMST mstBuilder;
	const double mstLength = TotalWeight(mstBuilder.Build(graph));

	const SteinerTreeBuilder steinerBuilder;
	const auto result = steinerBuilder.Build(terminals);
	REQUIRE(result.totalLength == Approx(mstLength));
	REQUIRE(result.vertices.size() == terminals.size());
}

TEST_CASE("InputReader parses well-formed input", "[io]")
{
	std::istringstream input{ "A 0 0\nB 3 4\nC 10 0\n" };
	const InputReader reader;
	const auto terminals = reader.Read(input);
	REQUIRE(terminals.size() == 3);
	REQUIRE(terminals[1].name == "B");
	REQUIRE(terminals[1].x == Approx(3.0));
	REQUIRE(terminals[1].y == Approx(4.0));
}

TEST_CASE("InputReader rejects malformed input", "[io]")
{
	std::istringstream input{ "A 0 0\nbroken\n" };
	const InputReader reader;
	REQUIRE_THROWS_AS(reader.Read(input), std::runtime_error);
}

TEST_CASE("Visualizer writes HTML with both SVG pictures", "[visualizer]")
{
	const std::vector<Point> terminals{
		{ "A", 0.0, 0.0 },
		{ "B", 1.0, 0.0 },
		{ "C", 0.5, std::sqrt(3.0) / 2.0 }
	};

	const auto completeGraph = Graph::BuildComplete(terminals);
	const BoruvkaMST mstBuilder;
	const auto mstEdges = mstBuilder.Build(completeGraph);

	const SteinerTreeBuilder steinerBuilder;
	const auto steinerResult = steinerBuilder.Build(terminals);

	const std::string path = "visualizer_test.html";
	Visualizer::SaveToHtml(terminals, mstEdges, steinerResult, path);

	std::ifstream file{ path };
	REQUIRE(file.is_open());
	const std::string html((std::istreambuf_iterator<char>{ file }),
		std::istreambuf_iterator<char>{});
	REQUIRE(html.find("<!DOCTYPE html>") != std::string::npos);
	REQUIRE(html.find("Minimum Spanning Tree") != std::string::npos);
	REQUIRE(html.find("Steiner Tree") != std::string::npos);
	// Two SVG blocks must be present.
	REQUIRE(html.find("<svg") != html.rfind("<svg"));
	// MST edges are blue, Steiner edges are red, Steiner points are green.
	REQUIRE(html.find("stroke='blue'") != std::string::npos);
	REQUIRE(html.find("stroke='red'") != std::string::npos);
	REQUIRE(html.find("fill='green'") != std::string::npos);
}

TEST_CASE("Visualizer throws on unopenable path", "[visualizer]")
{
	const std::vector<Point> terminals{ { "A", 0.0, 0.0 }, { "B", 1.0, 0.0 } };
	const auto graph = Graph::BuildComplete(terminals);
	const auto mstEdges = BoruvkaMST{}.Build(graph);
	const auto steinerResult = SteinerTreeBuilder{}.Build(terminals);

	REQUIRE_THROWS_AS(
		Visualizer::SaveToHtml(terminals, mstEdges, steinerResult,
			"/no/such/dir/out.html"),
		std::runtime_error);
}