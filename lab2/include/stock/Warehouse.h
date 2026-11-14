#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "catalog/Part.h"
#include "stock/StockItem.h"

namespace repair
{

/**
 * @brief Склад запчастей сервисного центра.
 *
 * Хранит по одной позиции на артикул. Все операции с остатками идут
 * через склад, а не через позиции напрямую: так в одном месте
 * собрана проверка того, что запчасть вообще заведена.
 */
class Warehouse
{
public:
    /** @brief Создаёт пустой склад. */
    Warehouse();

    /** @return Количество заведённых позиций. */
    std::size_t itemCount() const;

    /**
     * @brief Приходует запчасти, при необходимости заводя позицию.
     * @param part Поступившая запчасть.
     * @param quantity Поступившее количество.
     * @param location Место хранения.
     * @throws ValidationException Если количество не положительно.
     */
    void receive(const Part& part, int quantity, const std::string& location);

    /**
     * @brief Проверяет, заведена ли запчасть на складе.
     * @param part Проверяемая запчасть.
     * @return true, если позиция существует.
     */
    bool contains(const Part& part) const;

    /**
     * @brief Возвращает доступный к резервированию остаток.
     * @param part Запчасть.
     * @return Доступное количество.
     * @throws PartNotFoundException Если позиция не заведена.
     */
    int availableQuantity(const Part& part) const;

    /**
     * @brief Резервирует запчасти под заказ.
     * @param part Запчасть.
     * @param quantity Резервируемое количество.
     * @throws PartNotFoundException Если позиция не заведена.
     * @throws OutOfStockException Если остатка не хватает.
     */
    void reserve(const Part& part, int quantity);

    /**
     * @brief Снимает резерв.
     * @param part Запчасть.
     * @param quantity Освобождаемое количество.
     * @throws PartNotFoundException Если позиция не заведена.
     */
    void releaseReservation(const Part& part, int quantity);

    /**
     * @brief Списывает зарезервированные запчасти.
     * @param part Запчасть.
     * @param quantity Списываемое количество.
     * @throws PartNotFoundException Если позиция не заведена.
     */
    void writeOff(const Part& part, int quantity);

    /**
     * @brief Возвращает позицию по порядковому номеру.
     * @param index Номер позиции, начиная с нуля.
     * @return Позиция склада.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const StockItem& itemAt(std::size_t index) const;

    /**
     * @brief Собирает запчасти, остаток которых ниже порога пополнения.
     * @param minimum Порог пополнения.
     * @return Список запчастей, требующих дозаказа.
     */
    std::vector<Part> partsToReorder(int minimum) const;

private:
    StockItem& findItem(const Part& part);
    const StockItem& findItem(const Part& part) const;

    std::vector<StockItem> items_;
};

}
