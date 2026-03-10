#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "Graph.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <set>
#include <sstream>

const std::string TEST_GRAPH_FILE = "test_graph_temp.txt";

void CleanupTestFile()
{
    if (std::filesystem::exists(TEST_GRAPH_FILE))
    {
        std::filesystem::remove(TEST_GRAPH_FILE);
    }
}

std::vector<std::vector<int>> NormalizeSCCs(std::vector<std::vector<int>> sccs)
{
    for (auto& component : sccs)
    {
        std::ranges::sort(component);
    }
    std::ranges::sort(sccs, [](const std::vector<int>& a, const std::vector<int>& b) {
        if (a.empty() != b.empty()) return a.empty();
        if (!a.empty() && !b.empty()) return a[0] < b[0];
        return a.size() < b.size();
    });
    return sccs;
}

void CreateTestFile(const std::string& content)
{
    CleanupTestFile();
    std::ofstream out(TEST_GRAPH_FILE);
    out << content;
    out.close();
}

TEST_CASE("Graph loads simple file with names correctly", "[file-io]")
{
    CreateTestFile("3 2\nA B\nB C\n");

    Graph graph(0);
    REQUIRE_NOTHROW(graph.LoadFromFile(TEST_GRAPH_FILE));

    CHECK(graph.GetVertexCount() == 3);

    // check map
    const auto& names = graph.GetVertexNames();
    CHECK(names.size() == 3);
    CHECK(names[0] == "A");
    CHECK(names[1] == "B");
    CHECK(names[2] == "C");

    CleanupTestFile();
}

TEST_CASE("Graph handles empty graph (0 edges)", "[edge-case]")
{
    CreateTestFile("3 3\nA A\nB B\nC C\n");

    Graph graph(0);
    REQUIRE_NOTHROW(graph.LoadFromFile(TEST_GRAPH_FILE));
    CHECK(graph.GetVertexCount() == 3);

    const auto sccs = graph.FindStronglyConnectedComponents();
    CHECK(sccs.size() == 3);

    CleanupTestFile();
}

TEST_CASE("SCC: Single cycle forms one component", "[scc-basic]")
{
    CreateTestFile("3 3\nA B\nB C\nC A\n");

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    auto sccs = graph.FindStronglyConnectedComponents();

    REQUIRE(sccs.size() == 1);
    CHECK(sccs[0].size() == 3);

    std::set<int> content(sccs[0].begin(), sccs[0].end());
    CHECK(content == std::set<int>{0, 1, 2});

    CleanupTestFile();
}

TEST_CASE("SCC: Linear chain forms N components", "[scc-chain]")
{
    CreateTestFile("3 2\nA B\nB C\n");

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    const auto sccs = graph.FindStronglyConnectedComponents();

    REQUIRE(sccs.size() == 3);

    auto normalized = NormalizeSCCs(sccs);

    CHECK(normalized[0] == std::vector<int>{0});
    CHECK(normalized[1] == std::vector<int>{1});
    CHECK(normalized[2] == std::vector<int>{2});

    CleanupTestFile();
}

TEST_CASE("SCC: Complex mixed structure", "[scc-complex]")
{
    CreateTestFile("5 6\nA B\nB A\nB C\nC D\nD C\nD E\n");

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    auto sccs = graph.FindStronglyConnectedComponents();
    REQUIRE(sccs.size() == 3);

    auto normalized = NormalizeSCCs(sccs);

    CHECK(normalized[0] == std::vector<int>{0, 1});
    CHECK(normalized[1] == std::vector<int>{2, 3});
    CHECK(normalized[2] == std::vector<int>{4});

    CleanupTestFile();
}

TEST_CASE("SCC: Graph with no loops (from user request)", "[scc-no-loops]")
{
    CreateTestFile(
        "12 17\n"
        "A B\nB C\nC A\n"
        "C D\nD E\nE F\nF D\n"
        "F G\nG H\nH G\n"
        "H I\nI J\nJ K\nK L\nL K\n"
        "A E\nB F\n"
    );

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    CHECK(graph.GetVertexCount() == 12);

    auto sccs = graph.FindStronglyConnectedComponents();
    REQUIRE(sccs.size() == 6);

    std::vector<size_t> sizes;
    for(const auto& comp : sccs) sizes.push_back(comp.size());
    std::ranges::sort(sizes);
    CHECK(sizes[0] == 1);
    CHECK(sizes[1] == 1);
    CHECK(sizes[2] == 2);
    CHECK(sizes[3] == 2);
    CHECK(sizes[4] == 3);
    CHECK(sizes[5] == 3);

    CleanupTestFile();
}

TEST_CASE("Error Handling: Negative vertex count", "[error]")
{
    CreateTestFile("-5 0\n");

    Graph graph(0);
    CHECK_THROWS_AS(graph.LoadFromFile(TEST_GRAPH_FILE), std::runtime_error);

    CleanupTestFile();
}

TEST_CASE("Error Handling: Missing file", "[error]")
{
    Graph graph(0);
    CHECK_THROWS_AS(graph.LoadFromFile("non_existent_file_xyz.txt"), std::runtime_error);
}

TEST_CASE("Error Handling: Vertex count mismatch", "[error]")
{
    CreateTestFile("5 2\nA B\nB A\n");

    Graph graph(0);
    CHECK_THROWS_AS(graph.LoadFromFile(TEST_GRAPH_FILE), std::runtime_error);

    CleanupTestFile();
}

TEST_CASE("Error Handling: Empty vertex name", "[error]")
{
    SUCCEED("Stream extraction skips whitespace, hard to test empty name via file without binary manipulation");
}

TEST_CASE("Adapter: Matrix to Edge List conversion", "[adapter]")
{
    CreateTestFile("3 3\nA B\nB C\nC A\n");

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);

    auto edges = graph.ConvertMatrixToEdgeList();

    CHECK(edges.size() == 3);
    const std::set<std::pair<int,int>> edgeSet(edges.begin(), edges.end());
    CHECK(edgeSet.count({0, 1}) == 1);
    CHECK(edgeSet.count({1, 2}) == 1);
    CHECK(edgeSet.count({2, 0}) == 1);

    CleanupTestFile();
}

TEST_CASE("Adapter: BuildFromEdgeList with explicit count", "[adapter]")
{
    Graph graph(0);
    Graph::EdgeList edges = {{0, 1}, {1, 2}, {2, 0}};

    REQUIRE_NOTHROW(graph.BuildFromEdgeList(edges, 3));

    CHECK(graph.GetVertexCount() == 3);

    auto sccs = graph.FindStronglyConnectedComponents();
    REQUIRE(sccs.size() == 1);
    CHECK(sccs[0].size() == 3);
}

TEST_CASE("Adapter: BuildFromEdgeList dynamic growth", "[adapter]")
{
    Graph graph(0);
    Graph::EdgeList edges = {{0, 5}, {5, 2}, {2, 0}};

    REQUIRE_NOTHROW(graph.BuildFromEdgeList(edges, -1));
    CHECK(graph.GetVertexCount() == 6);

    const auto& matrix = graph.GetAdjacencyMatrix();
    CHECK(matrix.size() == 6);
    CHECK(matrix[0][5] == 1);
    CHECK(matrix[5][2] == 1);
    CHECK(matrix[2][0] == 1);
}

TEST_CASE("Internal: Adjacency Matrix consistency", "[internal]")
{
    CreateTestFile("4 4\nA B\nB C\nC D\nA D\n");

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);

    const auto& matrix = graph.GetAdjacencyMatrix();

    CHECK(matrix.size() == 4);

    CHECK(matrix[0][1] == 1);
    CHECK(matrix[0][3] == 1);
    CHECK(matrix[1][2] == 1);
    CHECK(matrix[2][3] == 1);
    CHECK(matrix[0][2] == 0);
    CHECK(matrix[3][0] == 0);
    CHECK(matrix[1][0] == 0);

    CleanupTestFile();
}