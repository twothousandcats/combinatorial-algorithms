#pragma once

#include "TreeTypes.h"
#include "ITreeBuilder.h"
#include <memory>
#include <vector>

class Repository
{
public:
	struct ComparisonResult
	{
		double mstLength{};
		double steinerLength{};
		double ratio{};
	};

	explicit Repository(
		std::unique_ptr<ITreeBuilder> mstBuilder,
		std::unique_ptr<ITreeBuilder> steinerBuilder
		);

	ComparisonResult Solve(const std::vector<Terminal>& terminals);

private:
	std::unique_ptr<ITreeBuilder> m_mstBuilder;
	std::unique_ptr<ITreeBuilder> m_steinerBuilder;

	static std::vector<Point> ExtractPoints(const std::vector<Terminal>& terminals);
};