#pragma once

#include <memory>

#include "finance/Invoice.h"
#include "people/Client.h"
#include "registry/EmployeeRegistry.h"
#include "registry/OrderBook.h"
#include "repair/IntakeAct.h"
#include "repair/RepairOrder.h"
#include "service/PricingService.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Оркестрация жизненного цикла заказа.
 *
 * Сервис без состояния. Связывает журнал заказов, реестр сотрудников
 * и расчёт стоимости: каждый из них по отдельности не знает о
 * других, а сценарий работы сервисного центра знает этот класс.
 */
class OrderService
{
public:
    /** @brief Создаёт сервис управления заказами. */
    OrderService();

    /**
     * @brief Открывает заказ и регистрирует его в журнале.
     * @param book Журнал заказов.
     * @param client Клиент.
     * @param intakeAct Акт приёмки.
     * @param openedOn Дата открытия.
     * @return Открытый заказ.
     * @throws ValidationException Если клиент не задан.
     */
    std::shared_ptr<RepairOrder> openOrder(OrderBook& book,
                                           const std::shared_ptr<Client>& client,
                                           const IntakeAct& intakeAct,
                                           const Date& openedOn) const;

    /**
     * @brief Подбирает и назначает на заказ подходящего мастера.
     * @param order Заказ-наряд.
     * @param registry Реестр сотрудников.
     * @throws NotFoundException Если свободного мастера нет.
     * @throws BusinessRuleException Если мастер уже назначен.
     */
    void assignBestTechnician(RepairOrder& order, const EmployeeRegistry& registry) const;

    /**
     * @brief Выставляет счёт по завершённому заказу.
     * @param order Заказ-наряд.
     * @param invoiceNumber Номер счёта.
     * @param issuedOn Дата выставления.
     * @return Выставленный счёт.
     * @throws BusinessRuleException Если смета не утверждена.
     */
    Invoice issueInvoice(const RepairOrder& order, int invoiceNumber,
                         const Date& issuedOn) const;

    /**
     * @brief Считает сумму к оплате по заказу через сервис расчёта.
     * @param order Заказ-наряд.
     * @param pricing Сервис расчёта стоимости.
     * @return Сумма к оплате с учётом скидки клиента.
     */
    Money amountDue(const RepairOrder& order, const PricingService& pricing) const;

    /**
     * @brief Проверяет, можно ли выдать устройство клиенту.
     * @param order Заказ-наряд.
     * @param invoice Счёт по заказу.
     * @return true, если ремонт завершён и счёт оплачен.
     */
    bool readyForIssue(const RepairOrder& order, const Invoice& invoice) const;
};

}
