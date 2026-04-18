#include "steiner_lab/input_validator.h"
#include "steiner_lab/exceptions.h"

#include <cmath>

namespace steiner_lab
{

// Проверяет конечность всех координат и нужна для отсечения NaN/inf до вычислений.
bool TerminalSetValidator::AreAllCoordinatesFinite(const std::vector<Point2D>& terminals) const noexcept
{
	for (const Point2D& point : terminals)
	{
		if (!std::isfinite(point.x) || !std::isfinite(point.y))
		{
			return false;
		}
	}
	return true;
}

// Бросает исключение при некорректных координатах и нужна для явной обработки ошибок ввода.
void TerminalSetValidator::ValidateAllCoordinatesFiniteOrThrow(const std::vector<Point2D>& terminals) const
{
	if (!AreAllCoordinatesFinite(terminals))
	{
		throw InvalidCoordinatesError("найдены нечисловые координаты (NaN или бесконечность)");
	}
}

} // namespace steiner_lab
