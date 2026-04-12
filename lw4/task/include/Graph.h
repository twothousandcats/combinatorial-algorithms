#pragma once
#include "Geometry.h"
#include <vector>
#include <memory>

struct Edge {
	int u{};
	int v{};
	double weight{};

	Edge() = default;
	Edge(int u, int v, double w) : u(u), v(v), weight(w) {}
};

struct Graph {
	std::vector<geometry::Point> nodes;
	std::vector<Edge> edges;

	void AddNode(const geometry::Point& p) {
		nodes.push_back(p);
	}

	void AddEdge(int u, int v) {
		if (u < 0 || v < 0 || u >= static_cast<int>(nodes.size()) || v >= static_cast<int>(nodes.size())) {
			return;
		}
		double w = geometry::Distance(nodes[u], nodes[v]);
		edges.emplace_back(u, v, w);
	}

	double TotalWeight() const {
		double sum = 0.0;
		for (const auto& e : edges) {
			sum += e.weight;
		}
		return sum;
	}
};