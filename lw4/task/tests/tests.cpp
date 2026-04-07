#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "BoruvkaTree.h"
#include "SteinerTree.h"
#include "Repository.h"
#include "Point.h"
#include <cmath>

namespace
{
    constexpr double EPSILON = 1e-6;
}

TEST_CASE("Distance calculation", "[Geometry]")
{
    Point a(0.0, 0.0);
    Point b(3.0, 4.0);

    REQUIRE(std::abs(Geometry::Distance(a, b) - 5.0) < EPSILON);
}

TEST_CASE("Centroid calculation", "[Geometry]")
{
    Point a(0.0, 0.0);
    Point b(3.0, 0.0);
    Point c(0.0, 3.0);

    Point centroid = Geometry::GetCentroid(a, b, c);

    REQUIRE(std::abs(centroid.x - 1.0) < EPSILON);
    REQUIRE(std::abs(centroid.y - 1.0) < EPSILON);
}

TEST_CASE("Boruvka MST with single point", "[Boruvka]")
{
    BoruvkaTree builder;
    std::vector<Point> points{{0.0, 0.0}};

    TreeResult result = builder.Build(points);

    REQUIRE(result.edges.empty());
    REQUIRE(result.totalLength == 0.0);
}

TEST_CASE("Boruvka MST with two points", "[Boruvka]")
{
    BoruvkaTree builder;
    std::vector<Point> points{{0.0, 0.0}, {3.0, 4.0}};

    TreeResult result = builder.Build(points);

    REQUIRE(result.edges.size() == 1);
    REQUIRE(std::abs(result.totalLength - 5.0) < EPSILON);
}

TEST_CASE("Boruvka MST with three points (equilateral triangle)", "[Boruvka]")
{
    BoruvkaTree builder;
    // Equilateral triangle with side length 1
    std::vector<Point> points{
        {0.0, 0.0},
        {1.0, 0.0},
        {0.5, std::sqrt(3.0) / 2.0}
    };

    TreeResult result = builder.Build(points);

    REQUIRE(result.edges.size() == 2);
    // MST should connect two edges of length 1
    REQUIRE(std::abs(result.totalLength - 2.0) < EPSILON);
}

TEST_CASE("Steiner Tree with single point", "[Steiner]")
{
    SteinerTree builder;
    std::vector<Point> points{{0.0, 0.0}};

    TreeResult result = builder.Build(points);

    REQUIRE(result.edges.empty());
    REQUIRE(result.totalLength == 0.0);
}

TEST_CASE("Steiner Tree with two points", "[Steiner]")
{
    SteinerTree builder;
    std::vector<Point> points{{0.0, 0.0}, {3.0, 4.0}};

    TreeResult result = builder.Build(points);

    REQUIRE(result.edges.size() == 1);
    REQUIRE(std::abs(result.totalLength - 5.0) < EPSILON);
}

TEST_CASE("Steiner Tree with three points (equilateral triangle)", "[Steiner]")
{
    SteinerTree builder;
    // Equilateral triangle with side length 1
    std::vector<Point> points{
        {0.0, 0.0},
        {1.0, 0.0},
        {0.5, std::sqrt(3.0) / 2.0}
    };

    TreeResult result = builder.Build(points);

    // Steiner tree should be shorter than MST (which is 2.0)
    REQUIRE(result.totalLength < 2.0);
    // The optimal Steiner tree for equilateral triangle is sqrt(3) ≈ 1.732
    REQUIRE(std::abs(result.totalLength - std::sqrt(3.0)) < 0.1);
}

TEST_CASE("Repository comparison", "[Repository]")
{
    auto repo = Repository(
        std::make_unique<BoruvkaTree>(),
        std::make_unique<SteinerTree>()
    );

    std::vector<Terminal> terminals{
        {"A", {0.0, 0.0}},
        {"B", {1.0, 0.0}},
        {"C", {0.5, std::sqrt(3.0) / 2.0}}
    };

    auto result = repo.Solve(terminals);

    REQUIRE(result.mstLength > 0);
    REQUIRE(result.steinerLength > 0);
    // Steiner tree should be shorter or equal to MST
    REQUIRE(result.steinerLength <= result.mstLength + EPSILON);
    // Ratio should be >= 1 (MST is always >= Steiner)
    REQUIRE(result.ratio >= 1.0 - EPSILON);
}

TEST_CASE("Empty terminals throws exception", "[Repository]")
{
    auto repo = Repository(
        std::make_unique<BoruvkaTree>(),
        std::make_unique<SteinerTree>()
    );

    std::vector<Terminal> terminals;

    REQUIRE_THROWS_AS(repo.Solve(terminals), std::invalid_argument);
}

TEST_CASE("Boruvka with four points (square)", "[Boruvka]")
{
    BoruvkaTree builder;
    // Square with side length 1
    std::vector<Point> points{
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0}
    };

    TreeResult result = builder.Build(points);

    REQUIRE(result.edges.size() == 3);
    // MST should connect three edges of length 1
    REQUIRE(std::abs(result.totalLength - 3.0) < EPSILON);
}

TEST_CASE("Steiner vs MST ratio for equilateral triangle", "[Comparison]")
{
    BoruvkaTree mstBuilder;
    SteinerTree steinerBuilder;

    std::vector<Point> points{
        {0.0, 0.0},
        {10.0, 0.0},
        {5.0, 10.0 * std::sqrt(3.0) / 2.0}
    };

    TreeResult mst = mstBuilder.Build(points);
    TreeResult steiner = steinerBuilder.Build(points);

    // For equilateral triangle, Steiner ratio is 2/sqrt(3) ≈ 1.155
    double ratio = mst.totalLength / steiner.totalLength;

    REQUIRE(steiner.totalLength < mst.totalLength);
    REQUIRE(ratio > 1.0);
}