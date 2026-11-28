#pragma once

#include <string>

#include "catalog/Part.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Резерв запчасти под конкретный заказ.
 *
 * Связывает склад и заказ-наряд: по резерву видно, кому обещаны
 * запчасти и когда. Снятый резерв не удаляется, а помечается
 * неактивным, чтобы в журнале осталась история.
 */
class PartReservation
{
public:
    /**
     * @brief Создаёт резерв.
     * @param part Резервируемая запчасть.
     * @param quantity Количество штук.
     * @param orderNumber Номер заказа, под который резервируется.
     * @param createdOn Дата создания резерва.
     * @throws ValidationException Если количество или номер заказа не положительны.
     */
    PartReservation(const Part& part, int quantity, int orderNumber, const Date& createdOn);

    /** @return Зарезервированная запчасть. */
    const Part& part() const;

    /** @return Количество штук. */
    int quantity() const;

    /** @return Номер заказа. */
    int orderNumber() const;

    /** @return Дата создания резерва. */
    const Date& createdOn() const;

    /** @return true, если резерв ещё действует. */
    bool isActive() const;

    /**
     * @brief Снимает резерв.
     * @throws BusinessRuleException Если резерв уже снят.
     */
    void release();

    /**
     * @brief Проверяет принадлежность резерва заказу.
     * @param candidateOrderNumber Номер проверяемого заказа.
     * @return true, если резерв создан под этот заказ.
     */
    bool belongsToOrder(int candidateOrderNumber) const;

    /**
     * @brief Проверяет, не залежался ли резерв.
     * @param today Дата проверки.
     * @param allowedDays Допустимый срок хранения резерва в днях.
     * @return true, если резерв старше допустимого срока и ещё активен.
     */
    bool isStale(const Date& today, int allowedDays) const;

    /** @return Строка резерва для журнала склада. */
    std::string describe() const;

private:
    Part part_;
    int quantity_;
    int orderNumber_;
    Date createdOn_;
    bool active_;
};

}
