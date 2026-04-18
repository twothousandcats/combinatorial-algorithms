#include "steiner_lab/input_parser.h"
#include "steiner_lab/exceptions.h"

#include <istream>
#include <sstream>
#include <string>

namespace steiner_lab
{

// Пытается разобрать одну строку как пару координат и нужна для валидации формата ввода.
bool TerminalSetParser::TryParseSingleLineAsTwoDoubles(const std::string& line, Point2D& outPoint)
{
	std::istringstream lineStream(line);
	double coordinateX = 0.0;
	double coordinateY = 0.0;
	if (!(lineStream >> coordinateX >> coordinateY))
	{
		return false;
	}
	lineStream >> std::ws;
	if (!lineStream.eof())
	{
		return false;
	}
	outPoint = {coordinateX, coordinateY};
	return true;
}

// Читает поток построчно в список точек и нужна для подготовки терминалов к алгоритмам.
std::vector<Point2D> TerminalSetParser::ParseLinesOfPairs(std::istream& input) const
{
	std::vector<Point2D> terminals;
	std::string line;
	std::size_t lineNumber = 0;
	while (std::getline(input, line))
	{
		++lineNumber;
		if (line.empty())
		{
			continue;
		}
		Point2D point{};
		if (!TryParseSingleLineAsTwoDoubles(line, point))
		{
			throw ParseError(lineNumber, "ожидались два числа с плавающей точкой на строку");
		}
		terminals.push_back(point);
	}
	return terminals;
}

} // namespace steiner_lab
