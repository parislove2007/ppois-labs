#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "stock/Supplier.h"
#include "stock/SupplyOrderItem.h"
#include "stock/Warehouse.h"
#include "value/Date.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Заказ запчастей у поставщика.
 *
 * После приёмки состав заказа фиксируется, а запчасти приходуются на
 * склад. Приёмка выполняется один раз: повторная означала бы двойное
 * оприходование одной поставки.
 */
class SupplyOrder
{
public:
    /**
     * @brief Создаёт пустой заказ поставщику.
     * @param number Номер заказа.
     * @param supplier Поставщик.
     * @param createdOn Дата оформления.
     * @throws ValidationException Если номер не положителен.
     */
    SupplyOrder(int number, const Supplier& supplier, const Date& createdOn);

    /** @return Номер заказа. */
    int number() const;

    /** @return Поставщик. */
    const Supplier& supplier() const;

    /** @return Дата оформления. */
    const Date& createdOn() const;

    /** @return Количество строк в заказе. */
    std::size_t itemCount() const;

    /** @return true, если поставка принята. */
    bool isReceived() const;

    /** @return Ожидаемая дата поставки. */
    Date expectedDate() const;

    /**
     * @brief Добавляет строку в заказ.
     * @param item Строка заказа.
     * @throws BusinessRuleException Если поставка уже принята.
     */
    void addItem(const SupplyOrderItem& item);

    /**
     * @brief Возвращает строку заказа по номеру.
     * @param index Номер строки, начиная с нуля.
     * @return Строка заказа.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const SupplyOrderItem& itemAt(std::size_t index) const;

    /** @return Суммарная стоимость закупки. */
    Money totalCost() const;

    /** @return Ожидаемая прибыль от всей партии. */
    Money expectedProfit() const;

    /**
     * @brief Принимает поставку и приходует запчасти на склад.
     * @param warehouse Склад, на который приходуются запчасти.
     * @param receivedOn Фактическая дата поставки.
     * @param location Место хранения для новых позиций.
     * @throws BusinessRuleException Если заказ пуст или уже принят.
     */
    void receive(Warehouse& warehouse, const Date& receivedOn, const std::string& location);

    /**
     * @brief Проверяет, пришла ли поставка в срок.
     * @return true, если фактическая дата не позже ожидаемой.
     * @throws BusinessRuleException Если поставка ещё не принята.
     */
    bool wasDeliveredOnTime() const;

private:
    int number_;
    Supplier supplier_;
    Date createdOn_;
    std::vector<SupplyOrderItem> items_;
    std::optional<Date> receivedOn_;
};

}
