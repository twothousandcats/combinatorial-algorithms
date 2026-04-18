#pragma once

#include "Geometry.h"

#include <istream>
#include <string>
#include <vector>

namespace steiner
{

class TerminalSetParser
{
public:
	[[nodiscard]] std::vector<Point2D> ParseLinesOfPairs(std::istream& input) const;

private:
	[[nodiscard]] static bool TryParseSingleLineAsTwoDoubles(const std::string& line, Point2D& outPoint);
};

} // namespace steiner