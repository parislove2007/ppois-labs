#pragma once

#include <string>

#include "repair/OrderStatus.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Запись о переходе заказа между состояниями.
 *
 * Хранится в заказе списком и служит журналом: по нему видно,
 * когда и кем менялось состояние.
 */
class OrderHistoryEntry
{
public:
    /**
     * @brief Создаёт запись журнала.
     * @param fromStatus Состояние до перехода.
     * @param toStatus Состояние после перехода.
     * @param changedOn Дата перехода.
     * @param changedBy Сотрудник, выполнивший переход.
     * @throws ValidationException Если сотрудник не указан.
     */
    OrderHistoryEntry(OrderStatus fromStatus, OrderStatus toStatus, const Date& changedOn,
                      const std::string& changedBy);

    /** @return Состояние до перехода. */
    OrderStatus fromStatus() const;

    /** @return Состояние после перехода. */
    OrderStatus toStatus() const;

    /** @return Дата перехода. */
    const Date& changedOn() const;

    /** @return Сотрудник, выполнивший переход. */
    const std::string& changedBy() const;

    /** @return Строка журнала для печати в акте. */
    std::string describe() const;

    /**
     * @brief Проверяет, относится ли запись к переходу в заданное состояние.
     * @param status Проверяемое состояние.
     * @return true, если переход был в это состояние.
     */
    bool leadsTo(OrderStatus status) const;

private:
    OrderStatus fromStatus_;
    OrderStatus toStatus_;
    Date changedOn_;
    std::string changedBy_;
};

}
