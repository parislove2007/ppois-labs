#pragma once

#include <string>

#include "catalog/Part.h"
#include "repair/PartUsage.h"
#include "stock/PartReservation.h"
#include "stock/Supplier.h"
#include "stock/SupplyOrder.h"
#include "stock/Warehouse.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Операции со складом в интересах заказов.
 *
 * Сервис без состояния. Связывает склад, резервы и заказы
 * поставщикам: склад умеет менять остатки, но не знает, под какой
 * заказ и зачем, а это знание живёт здесь.
 */
class InventoryService
{
public:
    /** @brief Создаёт сервис складских операций. */
    InventoryService();

    /**
     * @brief Проверяет, хватит ли запчастей на складе.
     * @param warehouse Склад.
     * @param part Запчасть.
     * @param quantity Требуемое количество.
     * @return true, если доступного остатка достаточно.
     */
    bool canSupply(const Warehouse& warehouse, const Part& part, int quantity) const;

    /**
     * @brief Резервирует запчасти под заказ и создаёт запись резерва.
     * @param warehouse Склад.
     * @param part Запчасть.
     * @param quantity Количество.
     * @param orderNumber Номер заказа.
     * @param createdOn Дата резервирования.
     * @return Созданная запись резерва.
     * @throws PartNotFoundException Если запчасть не заведена.
     * @throws OutOfStockException Если остатка не хватает.
     */
    PartReservation reserveForOrder(Warehouse& warehouse, const Part& part, int quantity,
                                    int orderNumber, const Date& createdOn) const;

    /**
     * @brief Списывает зарезервированные запчасти по факту установки.
     * @param warehouse Склад.
     * @param usage Запись об использовании запчасти.
     * @throws PartNotFoundException Если запчасть не заведена.
     */
    void consume(Warehouse& warehouse, const PartUsage& usage) const;

    /**
     * @brief Снимает резерв и возвращает запчасти в доступный остаток.
     * @param warehouse Склад.
     * @param reservation Снимаемый резерв.
     * @throws BusinessRuleException Если резерв уже снят.
     */
    void cancelReservation(Warehouse& warehouse, PartReservation& reservation) const;

    /**
     * @brief Формирует заказ поставщику на пополнение остатков.
     * @param warehouse Склад.
     * @param supplier Поставщик.
     * @param minimum Порог пополнения.
     * @param reorderQuantity Количество каждой позиции в дозаказе.
     * @param orderNumber Номер заказа поставщику.
     * @param createdOn Дата оформления.
     * @return Сформированный заказ поставщику.
     * @throws ValidationException Если количество дозаказа не положительно.
     */
    SupplyOrder buildReplenishment(const Warehouse& warehouse, const Supplier& supplier,
                                   int minimum, int reorderQuantity, int orderNumber,
                                   const Date& createdOn) const;
};

}
