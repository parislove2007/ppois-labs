#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Запись по указанному пути не найдена.
 */
class PathNotFoundException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit PathNotFoundException(const std::string& message);
};

}
