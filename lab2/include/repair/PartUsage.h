#pragma once

#include <string>

#include "catalog/Part.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Запчасть, списанная на выполнение заказа.
 *
 * Цена фиксируется в момент списания: каталог может подорожать,
 * но уже выставленный клиенту счёт от этого меняться не должен.
 */
class PartUsage
{
public:
    /**
     * @brief Создаёт запись о списании запчасти.
     * @param part Списанная запчасть.
     * @param quantity Количество штук.
     * @param unitPrice Цена за штуку на момент списания.
     * @throws ValidationException Если количество не положительно.
     */
    PartUsage(const Part& part, int quantity, const Money& unitPrice);

    /** @return Списанная запчасть. */
    const Part& part() const;

    /** @return Количество штук. */
    int quantity() const;

    /** @return Цена за штуку на момент списания. */
    const Money& unitPrice() const;

    /** @return Стоимость всей списанной партии. */
    Money totalCost() const;

    /**
     * @brief Проверяет, подорожала ли запчасть после списания.
     * @return true, если цена в каталоге выше зафиксированной.
     */
    bool priceRoseSinceUsage() const;

    /** @return Строка для акта выполненных работ. */
    std::string describe() const;

private:
    Part part_;
    int quantity_;
    Money unitPrice_;
};

}
