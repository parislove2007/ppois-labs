#pragma once

#include <string>
#include "error/RepairException.h"

namespace repair
{

/**
 * @brief Запрошенный объект не найден.
 *
 * Бросается реестрами и каталогами при поиске по несуществующему ключу.
 */
class NotFoundException : public RepairException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param message Текст сообщения об ошибке.
     */
    explicit NotFoundException(const std::string& message);
};

}
