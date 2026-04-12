#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "Geometry.h"
#include "Graph.h"
#include "MstBoruvka.h"
#include "SteinerTree.h"

using namespace geometry;

TEST_CASE("Point default construction", "[geometry]")
{
	Point p;
	REQUIRE(p.x == Catch::Approx(0.0));
	REQUIRE(p.y == Catch::Approx(0.0));
}

TEST_CASE("Point parameterized construction", "[geometry]")
{
	Point p(3.0, 4.0);
	REQUIRE(p.x == Catch::Approx(3.0));
	REQUIRE(p.y == Catch::Approx(4.0));
}

TEST_CASE("Point equality operator", "[geometry]")
{
	Point p1(1.0, 2.0);
	Point p2(1.0, 2.0);
	Point p3(1.0 + 1e-10, 2.0 + 1e-10);
	Point p4(2.0, 3.0);

	REQUIRE(p1 == p2);
	REQUIRE(p1 == p3); // Within epsilon
	REQUIRE(!(p1 == p4));
}

TEST_CASE("Distance calculation", "[geometry]")
{
	Point a(0.0, 0.0);
	Point b(3.0, 4.0);

	double dist = Distance(a, b);
	REQUIRE(dist == Catch::Approx(5.0));
}

TEST_CASE("Distance is symmetric", "[geometry]")
{
	Point a(1.0, 1.0);
	Point b(5.0, 5.0);

	REQUIRE(Distance(a, b) == Catch::Approx(Distance(b, a)));
}

TEST_CASE("Distance to self is zero", "[geometry]")
{
	Point p(7.5, -3.2);
	REQUIRE(Distance(p, p) == Catch::Approx(0.0));
}

TEST_CASE("Graph add node", "[graph]")
{
	Graph g;
	g.AddNode(Point(1.0, 2.0));
	g.AddNode(Point(3.0, 4.0));

	REQUIRE(g.nodes.size() == 2);
	REQUIRE(g.nodes[0].x == Catch::Approx(1.0));
	REQUIRE(g.nodes[1].y == Catch::Approx(4.0));
}

TEST_CASE("Graph add edge with valid indices", "[graph]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(3.0, 4.0));
	g.AddEdge(0, 1);

	REQUIRE(g.edges.size() == 1);
	REQUIRE(g.edges[0].u == 0);
	REQUIRE(g.edges[0].v == 1);
	REQUIRE(g.edges[0].weight == Catch::Approx(5.0));
}

TEST_CASE("Graph add edge with invalid indices", "[graph]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));

	// Should not crash, should not add edge
	g.AddEdge(-1, 0);
	g.AddEdge(0, 5);
	g.AddEdge(10, 20);

	REQUIRE(g.edges.empty());
}

TEST_CASE("Graph total weight empty", "[graph]")
{
	Graph g;
	REQUIRE(g.TotalWeight() == Catch::Approx(0.0));
}

TEST_CASE("Graph total weight with edges", "[graph]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(1.0, 0.0));
	g.AddNode(Point(2.0, 0.0));
	g.AddEdge(0, 1);
	g.AddEdge(1, 2);

	REQUIRE(g.TotalWeight() == Catch::Approx(2.0));
}

TEST_CASE("MST Boruvka single node", "[mst]")
{
	Graph g;
	g.AddNode(Point(5.0, 5.0));

	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(mst.nodes.size() == 1);
	REQUIRE(mst.edges.empty());
	REQUIRE(mst.TotalWeight() == Catch::Approx(0.0));
}

TEST_CASE("MST Boruvka two nodes", "[mst]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(3.0, 0.0));

	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(mst.nodes.size() == 2);
	REQUIRE(mst.edges.size() == 1);
	REQUIRE(mst.TotalWeight() == Catch::Approx(3.0));
}

TEST_CASE("MST Boruvka three nodes triangle", "[mst]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(4.0, 0.0));
	g.AddNode(Point(2.0, 3.0));

	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(mst.nodes.size() == 3);
	REQUIRE(mst.edges.size() == 2);
	// MST should connect all 3 points with minimum total weight
	REQUIRE(mst.TotalWeight() > 0.0);
}

TEST_CASE("MST Boruvka four nodes square", "[mst]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(1.0, 0.0));
	g.AddNode(Point(1.0, 1.0));
	g.AddNode(Point(0.0, 1.0));

	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(mst.nodes.size() == 4);
	REQUIRE(mst.edges.size() == 3);
	// MST of unit square should have weight 3.0 (three edges of length 1)
	REQUIRE(mst.TotalWeight() == Catch::Approx(3.0));
}

TEST_CASE("MST Boruvka empty graph", "[mst]")
{
	Graph g;
	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(mst.nodes.empty());
	REQUIRE(mst.edges.empty());
}

TEST_CASE("Steiner Tree single point", "[steiner]")
{
	Graph g;
	g.AddNode(Point(5.0, 5.0));

	auto result = SteinerTreeSolver::Compute(g);
	REQUIRE(result.graph.nodes.size() == 1);
	REQUIRE(result.length == Catch::Approx(0.0));
	REQUIRE(result.steinerPointsCount == 0);
}

TEST_CASE("Steiner Tree two points", "[steiner]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(4.0, 0.0));

	auto result = SteinerTreeSolver::Compute(g);
	REQUIRE(result.graph.nodes.size() == 2);
	REQUIRE(result.length == Catch::Approx(4.0));
	REQUIRE(result.steinerPointsCount == 0); // No Steiner point needed for 2 points
}

TEST_CASE("Steiner Tree equilateral triangle", "[steiner]")
{
	// For equilateral triangle, Steiner point reduces total length
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(10.0, 0.0));
	g.AddNode(Point(5.0, 10.0 * std::sqrt(3.0) / 2.0)); // Equilateral

	auto result = SteinerTreeSolver::Compute(g);
	REQUIRE(result.graph.nodes.size() >= 3);
	REQUIRE(result.length > 0.0);
	// Steiner tree should be no worse than MST
	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(result.length <= mst.TotalWeight() + 1e-6);
}

TEST_CASE("Steiner Tree colinear points", "[steiner]")
{
	// Colinear points: MST is optimal, no benefit from Steiner points
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(5.0, 0.0));
	g.AddNode(Point(10.0, 0.0));

	auto result = SteinerTreeSolver::Compute(g);
	REQUIRE(result.length == Catch::Approx(10.0)); // Direct line
	REQUIRE(result.steinerPointsCount == 0);
}

TEST_CASE("Steiner Tree result structure", "[steiner]")
{
	Graph g;
	g.AddNode(Point(0.0, 0.0));
	g.AddNode(Point(3.0, 0.0));
	g.AddNode(Point(0.0, 4.0));

	auto result = SteinerTreeSolver::Compute(g);

	REQUIRE(result.graph.nodes.size() >= 3);
	REQUIRE(result.length > 0.0);
	REQUIRE(result.steinerPointsCount >= 0);
}

TEST_CASE("GetTorricelliPoint obtuse triangle returns vertex", "[geometry]")
{
	// Triangle with angle > 120 degrees at A
	Point a(0.0, 0.0);
	Point b(1.0, 0.1);
	Point c(-1.0, 0.1);

	Point result = GetTorricelliPoint(a, b, c);
	// Should return vertex A since angle at A is obtuse (> 120)
	REQUIRE(result == a);
}

TEST_CASE("Distance negative coordinates", "[geometry]")
{
	Point a(-3.0, -4.0);
	Point b(0.0, 0.0);

	REQUIRE(Distance(a, b) == Catch::Approx(5.0));
}

TEST_CASE("Graph many nodes complete connectivity", "[mst]")
{
	const int N = 6;
	Graph g;
	for (int i = 0; i < N; ++i)
	{
		g.AddNode(Point(static_cast<double>(i), static_cast<double>(i * 2)));
	}

	Graph mst = MstBoruvka::Compute(g);
	REQUIRE(mst.nodes.size() == N);
	REQUIRE(mst.edges.size() == N - 1); // MST has n-1 edges
	REQUIRE(mst.TotalWeight() > 0.0);
}