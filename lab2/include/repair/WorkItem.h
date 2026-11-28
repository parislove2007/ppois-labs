#pragma once

#include <string>

#include "catalog/ServiceType.h"
#include "value/Duration.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Выполненная по заказу работа.
 *
 * Фиксирует факт: какая работа, кем, за какое время и по какой цене.
 * Цена берётся из прайса в момент выполнения и далее не меняется,
 * даже если прайс обновится.
 */
class WorkItem
{
public:
    /**
     * @brief Создаёт запись о выполненной работе.
     * @param serviceType Вид работы.
     * @param performedBy Фамилия и инициалы мастера.
     * @param spentTime Фактически затраченное время.
     * @param price Стоимость работы.
     * @throws ValidationException Если мастер не указан.
     */
    WorkItem(const ServiceType& serviceType, const std::string& performedBy,
             const Duration& spentTime, const Money& price);

    /** @return Вид работы. */
    const ServiceType& serviceType() const;

    /** @return Фамилия и инициалы мастера. */
    const std::string& performedBy() const;

    /** @return Фактически затраченное время. */
    const Duration& spentTime() const;

    /** @return Стоимость работы. */
    const Money& price() const;

    /**
     * @brief Проверяет, уложился ли мастер в норматив.
     * @return true, если фактическое время не превысило нормативное.
     */
    bool isWithinNorm() const;

    /**
     * @brief Считает превышение норматива.
     * @return Превышение; нулевая длительность, если норматив соблюдён.
     */
    Duration overtime() const;

    /** @return Строка для акта выполненных работ. */
    std::string describe() const;

private:
    ServiceType serviceType_;
    std::string performedBy_;
    Duration spentTime_;
    Money price_;
};

}
