#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Имя файла или каталога не соответствует правилам.
 */
class InvalidFileNameException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit InvalidFileNameException(const std::string& message);
};

}
