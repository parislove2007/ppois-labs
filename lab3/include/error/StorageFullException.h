#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Хранилище переполнено, свободное место закончилось.
 */
class StorageFullException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit StorageFullException(const std::string& message);
};

}
