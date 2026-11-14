#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "people/Client.h"
#include "people/Technician.h"
#include "repair/Diagnostics.h"
#include "repair/Estimate.h"
#include "repair/IntakeAct.h"
#include "repair/OrderHistoryEntry.h"
#include "repair/OrderStatus.h"
#include "repair/Warranty.h"
#include "value/Date.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Заказ-наряд на ремонт — центральная сущность модели.
 *
 * Связывает клиента, принятое устройство, назначенного мастера, смету
 * и гарантию. Состояние меняется только через методы этого класса, и
 * каждый переход проверяется по таблице допустимых: это не даёт,
 * например, выдать устройство, ремонт которого не начинался.
 *
 * Клиент хранится умным указателем, а не по значению, намеренно:
 * хранение по значению срезало бы корпоративного клиента до частного
 * и сломало бы расчёт договорной скидки.
 */
class RepairOrder
{
public:
    /**
     * @brief Создаёт заказ в состоянии «принят».
     * @param number Номер заказа, уникальный в журнале.
     * @param client Клиент, сдавший устройство.
     * @param intakeAct Акт приёмки.
     * @param openedOn Дата открытия заказа.
     * @throws ValidationException Если номер не положителен или клиент не задан.
     */
    RepairOrder(int number, const std::shared_ptr<Client>& client, const IntakeAct& intakeAct,
                const Date& openedOn);

    /** @return Номер заказа. */
    int number() const;

    /** @return Клиент, сдавший устройство. */
    const Client& client() const;

    /** @return Акт приёмки. */
    const IntakeAct& intakeAct() const;

    /** @return Текущее состояние заказа. */
    OrderStatus status() const;

    /** @return Дата открытия заказа. */
    const Date& openedOn() const;

    /** @return Смета по заказу. */
    const Estimate& estimate() const;

    /** @return Количество записей в журнале состояний. */
    std::size_t historySize() const;

    /**
     * @brief Возвращает запись журнала по номеру.
     * @param index Номер записи, начиная с нуля.
     * @return Запись журнала.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const OrderHistoryEntry& historyAt(std::size_t index) const;

    /** @return true, если заказ закрыт: выдан или отменён. */
    bool isClosed() const;

    /** @return true, если мастер назначен. */
    bool hasTechnician() const;

    /**
     * @brief Возвращает назначенного мастера.
     * @return Назначенный мастер.
     * @throws NotFoundException Если мастер не назначен.
     */
    const Technician& technician() const;

    /**
     * @brief Назначает мастера на заказ.
     * @param technician Назначаемый мастер.
     * @throws BusinessRuleException Если заказ закрыт, мастер уже назначен
     *         или у мастера нет нужной специализации.
     */
    void assignTechnician(const std::shared_ptr<Technician>& technician);

    /**
     * @brief Записывает результат диагностики и переводит заказ дальше.
     * @param diagnostics Результат диагностики.
     * @param performedOn Дата перехода.
     * @throws InvalidStatusTransitionException Если заказ не в состоянии «принят».
     */
    void recordDiagnostics(const Diagnostics& diagnostics, const Date& performedOn);

    /**
     * @brief Возвращает результат диагностики.
     * @return Результат диагностики.
     * @throws NotFoundException Если диагностика не проводилась.
     */
    const Diagnostics& diagnostics() const;

    /**
     * @brief Добавляет работу в смету.
     * @param work Выполняемая работа.
     * @throws InvalidStatusTransitionException Если смета уже не редактируется.
     */
    void addWorkToEstimate(const WorkItem& work);

    /**
     * @brief Добавляет запчасть в смету.
     * @param usage Списываемая запчасть.
     * @throws InvalidStatusTransitionException Если смета уже не редактируется.
     */
    void addPartToEstimate(const PartUsage& usage);

    /**
     * @brief Фиксирует утверждение сметы клиентом.
     * @param approvedOn Дата утверждения.
     * @throws InvalidStatusTransitionException Если заказ не продиагностирован.
     * @throws BusinessRuleException Если смета пуста.
     */
    void approveEstimate(const Date& approvedOn);

    /**
     * @brief Начинает ремонт.
     * @param startedOn Дата начала.
     * @throws InvalidStatusTransitionException Если смета не утверждена.
     * @throws BusinessRuleException Если мастер не назначен.
     */
    void startRepair(const Date& startedOn);

    /**
     * @brief Завершает ремонт и выдаёт гарантию.
     * @param completedOn Дата завершения.
     * @param warrantyMonths Срок гарантии в месяцах.
     * @throws InvalidStatusTransitionException Если ремонт не выполняется.
     */
    void completeRepair(const Date& completedOn, int warrantyMonths);

    /**
     * @brief Выдаёт устройство клиенту и закрывает заказ.
     * @param issuedOn Дата выдачи.
     * @throws InvalidStatusTransitionException Если заказ не готов к выдаче.
     */
    void issueToClient(const Date& issuedOn);

    /**
     * @brief Отменяет заказ.
     * @param cancelledOn Дата отмены.
     * @param reason Причина отмены.
     * @throws InvalidStatusTransitionException Если заказ уже закрыт.
     * @throws ValidationException Если причина не указана.
     */
    void cancel(const Date& cancelledOn, const std::string& reason);

    /**
     * @brief Возвращает гарантию на выполненный ремонт.
     * @return Гарантия.
     * @throws NotFoundException Если ремонт не завершён.
     */
    const Warranty& warranty() const;

    /** @return Итоговая стоимость заказа по смете. */
    Money totalCost() const;

private:
    void changeStatus(OrderStatus target, const Date& changedOn, const std::string& changedBy);
    static bool isTransitionAllowed(OrderStatus from, OrderStatus target);
    std::string actorName() const;

    int number_;
    std::shared_ptr<Client> client_;
    IntakeAct intakeAct_;
    OrderStatus status_;
    Date openedOn_;
    Estimate estimate_;
    std::vector<OrderHistoryEntry> history_;
    std::optional<Diagnostics> diagnostics_;
    std::optional<Warranty> warranty_;
    std::shared_ptr<Technician> technician_;
};

}
