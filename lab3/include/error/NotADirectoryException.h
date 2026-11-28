#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Операция требует каталог, а найдена другая запись.
 */
class NotADirectoryException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit NotADirectoryException(const std::string& message);
};

}
