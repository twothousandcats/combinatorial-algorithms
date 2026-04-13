#pragma once

struct Edge
{
	Edge() = default;

	Edge(int u, int v, double w);

	int u{};
	int v{};
	double weight{};
};