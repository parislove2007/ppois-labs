#pragma once

#include <stdexcept>
#include <string>

namespace repair
{

/**
 * @brief Базовое исключение предметной области.
 *
 * От него наследуются три ветви: нарушение формата входных данных,
 * отсутствие объекта и нарушение правила предметной области.
 */
class RepairException : public std::runtime_error
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit RepairException(const std::string& message);
};

}
