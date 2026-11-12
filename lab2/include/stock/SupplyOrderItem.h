#pragma once

#include <string>

#include "catalog/Part.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Строка заказа поставщику.
 *
 * Хранит закупочную цену отдельно от каталожной: продаём мы дороже,
 * чем покупаем, и для отчёта о прибыли нужны обе величины.
 */
class SupplyOrderItem
{
public:
    /**
     * @brief Создаёт строку заказа поставщику.
     * @param part Заказываемая запчасть.
     * @param quantity Количество штук.
     * @param purchasePrice Закупочная цена за штуку.
     * @throws ValidationException Если количество не положительно.
     */
    SupplyOrderItem(const Part& part, int quantity, const Money& purchasePrice);

    /** @return Заказываемая запчасть. */
    const Part& part() const;

    /** @return Количество штук. */
    int quantity() const;

    /** @return Закупочная цена за штуку. */
    const Money& purchasePrice() const;

    /** @return Стоимость строки заказа. */
    Money totalCost() const;

    /** @return Ожидаемая выручка от продажи партии по каталожной цене. */
    Money expectedRevenue() const;

    /** @return Ожидаемая прибыль от партии. */
    Money expectedProfit() const;

    /** @return Строка заказа для печати. */
    std::string describe() const;

private:
    Part part_;
    int quantity_;
    Money purchasePrice_;
};

}
