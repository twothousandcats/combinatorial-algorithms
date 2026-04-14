#include "InputReader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace steiner
{

std::vector<Point> InputReader::Read(std::istream& input) const
{
	std::vector<Point> terminals;
	std::string line;
	std::size_t lineNumber = 0;
	while (std::getline(input, line))
	{
		++lineNumber;
		if (line.find_first_not_of(" \t\r\n") == std::string::npos)
		{
			continue;
		}
		std::istringstream stream{ line };
		Point point{};
		long long xCoord = 0;
		long long yCoord = 0;
		if (!(stream >> point.name >> xCoord >> yCoord))
		{
			throw std::runtime_error{
				"InputReader: invalid format at line " + std::to_string(lineNumber) };
		}
		point.x = static_cast<double>(xCoord);
		point.y = static_cast<double>(yCoord);
		terminals.push_back(std::move(point));
	}
	return terminals;
}

std::vector<Point> InputReader::ReadFromFile(const std::string& path) const
{
	std::ifstream file{ path };
	if (!file.is_open())
	{
		throw std::runtime_error{ "InputReader: cannot open file '" + path + "'" };
	}
	return Read(file);
}

} // namespace steiner