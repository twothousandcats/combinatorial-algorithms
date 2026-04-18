#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace steiner
{

class SteinerException : public std::runtime_error
{
public:
	explicit SteinerException(std::string message)
		: std::runtime_error(std::move(message))
	{
	}
};

class ParseError final : public SteinerException
{
public:
	ParseError(std::size_t lineNumber, std::string message)
		: SteinerException("строка " + std::to_string(lineNumber) + ": " + std::move(message))
		  , m_lineNumber(lineNumber)
	{
	}

	[[nodiscard]] std::size_t LineNumber() const noexcept
	{
		return m_lineNumber;
	}

private:
	std::size_t m_lineNumber;
};

class IoError final : public SteinerException
{
public:
	explicit IoError(std::string message)
		: SteinerException(std::move(message))
	{
	}
};

class InvalidCoordinatesError final : public SteinerException
{
public:
	explicit InvalidCoordinatesError(std::string message)
		: SteinerException(std::move(message))
	{
	}
};

} // namespace steiner