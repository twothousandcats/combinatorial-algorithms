#pragma once

#include "steiner_lab/geometry.h"
#include <istream>
#include <string>
#include <vector>

namespace steiner_lab
{

class TerminalSetParser
{
public:
	[[nodiscard]] std::vector<Point2D> ParseLinesOfPairs(std::istream& input) const;

private:
	[[nodiscard]] static bool TryParseSingleLineAsTwoDoubles(const std::string& line, Point2D& outPoint);
};

} // namespace steiner_lab
