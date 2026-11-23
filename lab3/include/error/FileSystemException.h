#pragma once

#include <stdexcept>
#include <string>

namespace fs
{

/**
 * @brief Базовое исключение домена файловой системы.
 *
 * От него наследуются все специализированные исключения: некорректные
 * имена и размеры, отсутствие записей, нарушение прав, переполнение
 * хранилища и конфликты версий.
 */
class FileSystemException : public std::runtime_error
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit FileSystemException(const std::string& message);
};

}
