#include "DisjointSet.h"

#include <numeric>

namespace steiner
{

DisjointSet::DisjointSet(std::size_t size)
	: m_parent(size)
	  , m_rank(size, 0)
{
	std::iota(m_parent.begin(), m_parent.end(), std::size_t{ 0 });
}

std::size_t DisjointSet::Find(std::size_t x)
{
	while (m_parent[x] != x)
	{
		m_parent[x] = m_parent[m_parent[x]]; // path compression (halving)
		x = m_parent[x];
	}
	return x;
}

bool DisjointSet::Unite(std::size_t a, std::size_t b)
{
	const std::size_t ra = Find(a);
	const std::size_t rb = Find(b);
	if (ra == rb)
	{
		return false;
	}
	if (m_rank[ra] < m_rank[rb])
	{
		m_parent[ra] = rb;
	}
	else if (m_rank[ra] > m_rank[rb])
	{
		m_parent[rb] = ra;
	}
	else
	{
		m_parent[rb] = ra;
		++m_rank[ra];
	}
	return true;
}

} // namespace steiner