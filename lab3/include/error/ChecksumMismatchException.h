#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Контрольная сумма содержимого не совпадает.
 */
class ChecksumMismatchException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit ChecksumMismatchException(const std::string& message);
};

}
