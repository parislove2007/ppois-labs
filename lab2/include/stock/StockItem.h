#pragma once

#include <string>

#include "catalog/Part.h"

namespace repair
{

/**
 * @brief Позиция склада: запчасть, её остаток и место хранения.
 *
 * Различает общий остаток и зарезервированное количество. Резерв —
 * это запчасти, физически лежащие на складе, но уже обещанные
 * конкретным заказам, поэтому выдавать их другим нельзя.
 */
class StockItem
{
public:
    /**
     * @brief Создаёт позицию склада.
     * @param part Хранимая запчасть.
     * @param quantity Начальный остаток.
     * @param location Место хранения в виде "стеллаж-полка-ячейка".
     * @throws ValidationException Если остаток отрицателен или место не указано.
     */
    StockItem(const Part& part, int quantity, const std::string& location);

    /** @return Хранимая запчасть. */
    const Part& part() const;

    /** @return Общий остаток на складе. */
    int quantity() const;

    /** @return Количество, обещанное заказам. */
    int reservedQuantity() const;

    /** @return Место хранения. */
    const std::string& location() const;

    /** @return Остаток, доступный к резервированию. */
    int availableQuantity() const;

    /**
     * @brief Приходует запчасти на склад.
     * @param amount Поступившее количество.
     * @throws ValidationException Если количество не положительно.
     */
    void receive(int amount);

    /**
     * @brief Резервирует запчасти под заказ.
     * @param amount Резервируемое количество.
     * @throws OutOfStockException Если доступного остатка не хватает.
     */
    void reserve(int amount);

    /**
     * @brief Снимает резерв.
     * @param amount Освобождаемое количество.
     * @throws BusinessRuleException Если столько не зарезервировано.
     */
    void releaseReservation(int amount);

    /**
     * @brief Списывает зарезервированные запчасти со склада.
     * @param amount Списываемое количество.
     * @throws BusinessRuleException Если столько не зарезервировано.
     */
    void writeOff(int amount);

    /**
     * @brief Проверяет, не опустился ли остаток ниже порога пополнения.
     * @param minimum Порог пополнения.
     * @return true, если остаток меньше порога.
     */
    bool isBelowMinimum(int minimum) const;

private:
    Part part_;
    int quantity_;
    int reserved_;
    std::string location_;
};

}
