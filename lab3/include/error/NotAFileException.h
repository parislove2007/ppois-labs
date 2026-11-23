#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Операция требует файл, а найдена другая запись.
 */
class NotAFileException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit NotAFileException(const std::string& message);
};

}
