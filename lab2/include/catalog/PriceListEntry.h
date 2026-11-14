#pragma once

#include <iosfwd>

#include "catalog/ServiceType.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Строка прайс-листа: вид работы и его цена.
 *
 * Справочная запись. Выделена в отдельный класс, чтобы прайс-лист
 * оставался простой коллекцией, а правила расчёта стоимости партии
 * работ лежали рядом с ценой.
 */
class PriceListEntry
{
public:
    /**
     * @brief Создаёт строку прайса.
     * @param serviceType Вид работы.
     * @param price Цена за одну операцию.
     */
    PriceListEntry(const ServiceType& serviceType, const Money& price);

    /** @return Вид работы. */
    const ServiceType& serviceType() const;

    /** @return Цена за одну операцию. */
    const Money& price() const;

    /**
     * @brief Считает стоимость нескольких операций.
     * @param repetitions Число повторений работы.
     * @return Суммарная стоимость.
     * @throws ValidationException Если число повторений не положительно.
     */
    Money totalFor(int repetitions) const;

    /**
     * @brief Проверяет, относится ли строка к заданному виду работы.
     * @param candidate Проверяемый вид работы.
     * @return true, если виды работ совпадают.
     */
    bool describes(const ServiceType& candidate) const;

private:
    ServiceType serviceType_;
    Money price_;
};

/** @brief Записывает строку прайса в поток. */
std::ostream& operator<<(std::ostream& out, const PriceListEntry& entry);

}
