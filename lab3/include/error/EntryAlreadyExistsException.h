#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Попытка создать запись с уже занятым именем.
 */
class EntryAlreadyExistsException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit EntryAlreadyExistsException(const std::string& message);
};

}
