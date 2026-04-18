#include "steiner_lab/file_io.h"
#include "steiner_lab/exceptions.h"

#include <fstream>

namespace steiner_lab
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

} // namespace steiner_lab
