#include "Repository.h"

#include <memory>
#include <stdexcept>

Repository::Repository(
	std::unique_ptr<ITreeBuilder> mstBuilder,
	std::unique_ptr<ITreeBuilder> steinerBuilder)
	: m_mstBuilder(std::move(mstBuilder))
	, m_steinerBuilder(std::move(steinerBuilder))
{
}

std::vector<Point> Repository::ExtractPoints(const std::vector<Terminal>& terminals)
{
	std::vector<Point> points;
	points.reserve(terminals.size());
	for (const auto& t : terminals)
	{
		points.push_back(t.position);
	}
	return points;
}

Repository::ComparisonResult Repository::Solve(const std::vector<Terminal>& terminals)
{
	if (terminals.empty())
	{
		throw std::invalid_argument("Terminals list cannot be empty");
	}

	const auto points = ExtractPoints(terminals);

	const TreeResult mst = m_mstBuilder->Build(points);
	const TreeResult steiner = m_steinerBuilder->Build(points);

	ComparisonResult res;
	res.mstLength = mst.totalLength;
	res.steinerLength = steiner.totalLength;
    
	if (steiner.totalLength > 1e-9)
	{
		res.ratio = mst.totalLength / steiner.totalLength;
	}
	else
	{
		res.ratio = 1.0;
	}

	return res;
}