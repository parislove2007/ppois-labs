#pragma once

#include <string>
#include "error/FileSystemException.h"

namespace fs
{

/**
 * @brief Попытка восстановить запись из пустой корзины.
 */
class EmptyTrashException : public FileSystemException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit EmptyTrashException(const std::string& message);
};

}
