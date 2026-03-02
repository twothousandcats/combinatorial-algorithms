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

// Нормализация результата SCC для сравнения:
// Сортирует вершины внутри каждой компоненты
// Сортирует сами компоненты по первому элементу
std::vector<std::vector<int>> NormalizeSCCs(std::vector<std::vector<int>> sccs)
{
    for (auto& component : sccs)
    {
        std::ranges::sort(component);
    }
    std::ranges::sort(sccs, [](const std::vector<int>& a, const std::vector<int>& b) {
        return a.empty() || b.empty() ? a.size() < b.size() : a[0] < b[0];
    });
    return sccs;
}

TEST_CASE("Graph loads simple file correctly", "[file-io]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "3 2\n";
        out << "A B\n";
        out << "B C\n";
    }

    Graph graph(0);
    REQUIRE_NOTHROW(graph.LoadFromFile(TEST_GRAPH_FILE));

    CHECK(graph.GetVertexCount() == 3);

    CleanupTestFile();
}

TEST_CASE("Graph handles empty graph (0 edges)", "[edge-case]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "5 0\n";
    }

    Graph graph(0);
    REQUIRE_NOTHROW(graph.LoadFromFile(TEST_GRAPH_FILE));
    CHECK(graph.GetVertexCount() == 5);

    auto sccs = graph.FindStronglyConnectedComponents();
    CHECK(sccs.size() == 5);

    CleanupTestFile();
}

TEST_CASE("SCC: Single cycle forms one component", "[scc-basic]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "3 3\n";
        out << "0 1\n";
        out << "1 2\n";
        out << "2 0\n";
    }

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
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "3 2\n";
        out << "0 1\n";
        out << "1 2\n";
    }

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    auto sccs = graph.FindStronglyConnectedComponents();

    REQUIRE(sccs.size() == 3);

    auto normalized = NormalizeSCCs(sccs);

    CHECK(normalized[0] == std::vector<int>{0});
    CHECK(normalized[1] == std::vector<int>{1});
    CHECK(normalized[2] == std::vector<int>{2});

    CleanupTestFile();
}

TEST_CASE("SCC: Complex mixed structure", "[scc-complex]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "5 6\n";
        out << "0 1\n";
        out << "1 0\n";
        out << "1 2\n";
        out << "2 3\n";
        out << "3 2\n";
        out << "3 4\n";
    }

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    auto sccs = graph.FindStronglyConnectedComponents();

    REQUIRE(sccs.size() == 3);

    auto normalized = NormalizeSCCs(sccs);

    // Ожидаем компоненты: {0,1}, {2,3}, {4}
    CHECK(normalized[0] == std::vector<int>{0, 1});
    CHECK(normalized[1] == std::vector<int>{2, 3});
    CHECK(normalized[2] == std::vector<int>{4});

    CleanupTestFile();
}

TEST_CASE("SCC: Graph with no loops (from user request)", "[scc-no-loops]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "12 17\n";
        out << "A B\nB C\nC A\n"; // SCC {A,B,C}
        out << "C D\nD E\nE F\nF D\n"; // SCC {D,E,F}
        out << "F G\nG H\nH G\n"; // SCC {G,H}
        out << "H I\nI J\nJ K\nK L\nL K\n"; // SCC {I,J,K,L} - цикл через L->K
        out << "A E\nB F\n"; // Перекрестные ребра
    }

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);
    CHECK(graph.GetVertexCount() == 12);

    auto sccs = graph.FindStronglyConnectedComponents();
    REQUIRE(sccs.size() == 4);

    std::vector<size_t> sizes;
    for(const auto& comp : sccs) sizes.push_back(comp.size());
    std::ranges::sort(sizes);

    CHECK(sizes[0] == 2); // {G,H}
    CHECK(sizes[1] == 3); // {A,B,C} или {D,E,F}
    CHECK(sizes[2] == 3);
    CHECK(sizes[3] == 4); // {I,J,K,L}

    CleanupTestFile();
}

TEST_CASE("Error Handling: Negative vertex count", "[error]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "-5 0\n";
    }

    Graph graph(0);
    CHECK_THROWS_AS(graph.LoadFromFile(TEST_GRAPH_FILE), std::invalid_argument);

    CleanupTestFile();
}

TEST_CASE("Error Handling: Missing file", "[error]")
{
    Graph graph(0);
    CHECK_THROWS_AS(graph.LoadFromFile("non_existent_file_xyz.txt"), std::runtime_error);
}

TEST_CASE("Error Handling: Premature EOF", "[error]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "5 10\n";
        out << "0 1\n";
    }

    Graph graph(0);
    CHECK_THROWS_AS(graph.LoadFromFile(TEST_GRAPH_FILE), std::runtime_error);

    CleanupTestFile();
}

TEST_CASE("Internal: Adjacency Matrix consistency", "[internal]")
{
    CleanupTestFile();
    {
        std::ofstream out(TEST_GRAPH_FILE);
        out << "4 4\n";
        out << "0 1\n";
        out << "1 2\n";
        out << "2 3\n";
        out << "0 3\n";
    }

    Graph graph(0);
    graph.LoadFromFile(TEST_GRAPH_FILE);

    const auto& matrix = graph.GetAdjacencyMatrix();

    CHECK(matrix.size() == 4);
    CHECK(matrix[0][1] == 1);
    CHECK(matrix[0][3] == 1);
    CHECK(matrix[1][2] == 1);
    CHECK(matrix[2][3] == 1);

    // Проверка отсутствия связей
    CHECK(matrix[0][2] == 0);
    CHECK(matrix[3][0] == 0);
    CHECK(matrix[1][0] == 0);

    CleanupTestFile();
}