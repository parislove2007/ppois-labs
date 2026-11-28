#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Конфликт версий при обновлении записи.
 */
class VersionConflictException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit VersionConflictException(const std::string& message);
};

}
