#pragma once

#include <string>
#include "error/RepairException.h"

namespace repair
{

/**
 * @brief Нарушено правило предметной области.
 *
 * Бросается сущностями и сервисами, когда операция сама по себе
 * корректна, но запрещена текущим состоянием модели.
 */
class BusinessRuleException : public RepairException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit BusinessRuleException(const std::string& message);
};

}
