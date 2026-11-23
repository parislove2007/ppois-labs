#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Недостаточно прав для выполнения операции.
 */
class PermissionDeniedException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit PermissionDeniedException(const std::string& message);
};

}
