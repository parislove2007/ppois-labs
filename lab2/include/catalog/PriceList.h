#pragma once

#include <cstddef>
#include <vector>

#include "catalog/PriceListEntry.h"
#include "catalog/ServiceType.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Действующий прайс-лист сервисного центра.
 *
 * Хранит по одной строке на вид работы. Повторное добавление того же
 * вида работы заменяет цену: прайс не может содержать два разных
 * значения для одной услуги.
 */
class PriceList
{
public:
    /** @brief Создаёт пустой прайс-лист. */
    PriceList();

    /** @return Количество строк в прайсе. */
    std::size_t size() const;

    /** @return true, если прайс пуст. */
    bool isEmpty() const;

    /**
     * @brief Добавляет или обновляет цену вида работы.
     * @param serviceType Вид работы.
     * @param price Цена за одну операцию.
     */
    void setPrice(const ServiceType& serviceType, const Money& price);

    /**
     * @brief Проверяет наличие вида работы в прайсе.
     * @param serviceType Проверяемый вид работы.
     * @return true, если цена задана.
     */
    bool contains(const ServiceType& serviceType) const;

    /**
     * @brief Возвращает цену вида работы.
     * @param serviceType Вид работы.
     * @return Цена за одну операцию.
     * @throws NotFoundException Если цена не задана.
     */
    Money priceFor(const ServiceType& serviceType) const;

    /**
     * @brief Считает стоимость партии одинаковых работ.
     * @param serviceType Вид работы.
     * @param repetitions Число повторений.
     * @return Суммарная стоимость.
     * @throws NotFoundException Если цена не задана.
     * @throws ValidationException Если число повторений не положительно.
     */
    Money totalFor(const ServiceType& serviceType, int repetitions) const;

    /**
     * @brief Возвращает строку прайса по порядковому номеру.
     * @param index Номер строки, начиная с нуля.
     * @return Строка прайса.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const PriceListEntry& entryAt(std::size_t index) const;

private:
    std::vector<PriceListEntry> entries_;
};

}
