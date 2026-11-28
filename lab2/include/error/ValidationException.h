#pragma once

#include <string>
#include "error/RepairException.h"

namespace repair
{

/**
 * @brief Некорректные входные данные.
 *
 * Бросается из конструкторов объектов-значений, когда переданное
 * значение не проходит проверку инвариантов.
 */
class ValidationException : public RepairException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit ValidationException(const std::string& message);
};

}
