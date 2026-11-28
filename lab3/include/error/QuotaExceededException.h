#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Превышена дисковая квота пользователя.
 */
class QuotaExceededException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit QuotaExceededException(const std::string& message);
};

}
