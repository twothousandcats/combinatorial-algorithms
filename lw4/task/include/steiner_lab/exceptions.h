#pragma once

#include <stdexcept>
#include <string>

namespace steiner_lab
{

class SteinerLabException : public std::runtime_error
{
public:
	explicit SteinerLabException(std::string message) : std::runtime_error(std::move(message))
	{
	}
};

class ParseError final : public SteinerLabException
{
public:
	ParseError(std::size_t lineNumber, std::string message)
		: SteinerLabException("строка " + std::to_string(lineNumber) + ": " + std::move(message))
		, m_lineNumber(lineNumber)
	{
	}

	[[nodiscard]] std::size_t LineNumber() const noexcept { return m_lineNumber; }

private:
	std::size_t m_lineNumber;
};

class IoError final : public SteinerLabException
{
public:
	explicit IoError(std::string message) : SteinerLabException(std::move(message)) {}
};

class InvalidCoordinatesError final : public SteinerLabException
{
public:
	explicit InvalidCoordinatesError(std::string message) : SteinerLabException(std::move(message)) {}
};

} // namespace steiner_lab
