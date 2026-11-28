#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "repair/OrderStatus.h"
#include "repair/RepairOrder.h"

namespace repair
{

/**
 * @brief Журнал заказов-нарядов.
 *
 * Выдаёт номера заказов и хранит их до закрытия. Номер выдаётся
 * журналом, а не вызывающим кодом: так гарантируется, что два заказа
 * не получат один номер.
 */
class OrderBook
{
public:
    /** @brief Создаёт пустой журнал. */
    OrderBook();

    /** @return Количество заказов в журнале. */
    std::size_t size() const;

    /** @return Номер, который получит следующий заказ. */
    int nextNumber() const;

    /**
     * @brief Выдаёт очередной номер заказа.
     * @return Номер для нового заказа.
     */
    int issueNumber();

    /**
     * @brief Помещает заказ в журнал.
     * @param order Регистрируемый заказ.
     * @throws ValidationException Если заказ не задан.
     * @throws BusinessRuleException Если номер уже занят.
     */
    void add(const std::shared_ptr<RepairOrder>& order);

    /**
     * @brief Находит заказ по номеру.
     * @param number Номер заказа.
     * @return Найденный заказ.
     * @throws OrderNotFoundException Если заказа с таким номером нет.
     */
    std::shared_ptr<RepairOrder> findByNumber(int number) const;

    /**
     * @brief Проверяет наличие заказа с номером.
     * @param number Номер заказа.
     * @return true, если заказ есть в журнале.
     */
    bool contains(int number) const;

    /**
     * @brief Возвращает заказ по порядковому номеру в журнале.
     * @param index Номер, начиная с нуля.
     * @return Заказ.
     * @throws NotFoundException Если номер выходит за границы.
     */
    std::shared_ptr<RepairOrder> orderAt(std::size_t index) const;

    /**
     * @brief Собирает заказы в заданном состоянии.
     * @param status Искомое состояние.
     * @return Список подходящих заказов.
     */
    std::vector<std::shared_ptr<RepairOrder>> ordersWithStatus(OrderStatus status) const;

    /** @return Количество незакрытых заказов. */
    std::size_t openOrderCount() const;

private:
    std::vector<std::shared_ptr<RepairOrder>> orders_;
    int nextNumber_;
};

}
