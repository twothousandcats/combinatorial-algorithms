#pragma once

#include "Point.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace steiner
{

// Reads terminal definitions in the form "name x y" (one per line).
class InputReader
{
public:
	std::vector<Point> Read(std::istream& input) const;

	std::vector<Point> ReadFromFile(const std::string& path) const;
};

} // namespace steiner