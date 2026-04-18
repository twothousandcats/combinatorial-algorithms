#include "FileIo.h"
#include "Exceptions.h"

#include <fstream>

namespace steiner
{

// Записывает текст в файл и нужен для сохранения результатов/артефактов на диск.
void WriteTextFile(const std::string& path, const std::string& text)
{
	std::ofstream fileStream(path);
	if (!fileStream.is_open())
	{
		throw IoError("не удалось открыть файл для записи: " + path);
	}
	fileStream << text;
	if (!fileStream.good())
	{
		throw IoError("ошибка записи в файл: " + path);
	}
}

} // namespace steiner