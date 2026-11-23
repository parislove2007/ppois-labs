#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Размер файла некорректен: отрицательный или переполненный.
 */
class InvalidFileSizeException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit InvalidFileSizeException(const std::string& message);
};

}
