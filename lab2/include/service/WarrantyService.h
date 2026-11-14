#pragma once

#include "repair/RepairOrder.h"
#include "repair/WarrantyClaim.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Расчёт срока гарантии и разбор гарантийных обращений.
 *
 * Сервис без состояния. Срок гарантии зависит одновременно от
 * категории техники и от сложности выполненного ремонта, поэтому
 * его расчёт не принадлежит ни категории, ни заказу по отдельности.
 */
class WarrantyService
{
public:
    /** @brief Создаёт сервис гарантийного обслуживания. */
    WarrantyService();

    /**
     * @brief Считает срок гарантии на выполненный ремонт.
     * @param order Заказ-наряд.
     * @return Срок гарантии в месяцах.
     * @throws NotFoundException Если диагностика по заказу не проводилась.
     */
    int warrantyMonthsFor(const RepairOrder& order) const;

    /**
     * @brief Проверяет, является ли обращение гарантийным случаем.
     * @param order Исходный заказ.
     * @param claim Гарантийное обращение.
     * @return true, если гарантия действует и покрывает узел.
     * @throws NotFoundException Если гарантия по заказу не выдана.
     */
    bool isCoveredCase(const RepairOrder& order, const WarrantyClaim& claim) const;

    /**
     * @brief Выносит решение по гарантийному обращению.
     * @param order Исходный заказ.
     * @param claim Рассматриваемое обращение.
     * @throws BusinessRuleException Если решение уже принято.
     * @throws NotFoundException Если гарантия по заказу не выдана.
     */
    void process(const RepairOrder& order, WarrantyClaim& claim) const;

    /**
     * @brief Возвращает дату окончания гарантии по заказу.
     * @param order Заказ-наряд.
     * @return Дата окончания гарантии.
     * @throws NotFoundException Если гарантия не выдана.
     */
    Date coverageEnd(const RepairOrder& order) const;
};

}
