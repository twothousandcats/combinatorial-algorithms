#pragma once

#include <cstddef>
#include <vector>

namespace steiner
{

class DisjointSet
{
public:
	explicit DisjointSet(std::size_t size);

	std::size_t Find(std::size_t x);

	bool Unite(std::size_t a, std::size_t b);

private:
	std::vector<std::size_t> m_parent;
	std::vector<std::size_t> m_rank;
};

} // namespace steiner