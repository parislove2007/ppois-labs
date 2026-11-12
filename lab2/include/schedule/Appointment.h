#pragma once

#include <memory>
#include <string>

#include "people/Client.h"
#include "schedule/TimeSlot.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Запись клиента на приём в сервисный центр.
 *
 * Клиент хранится умным указателем, чтобы запись на приём
 * корпоративного клиента не теряла его тип. Отменённую запись нельзя
 * перенести: вместо этого оформляется новая.
 */
class Appointment
{
public:
    /**
     * @brief Создаёт запись на приём.
     * @param client Записывающийся клиент.
     * @param date Дата приёма.
     * @param slot Интервал приёма.
     * @param purpose Цель визита.
     * @throws ValidationException Если клиент не задан или цель не указана.
     */
    Appointment(const std::shared_ptr<Client>& client, const Date& date, const TimeSlot& slot,
                const std::string& purpose);

    /** @return Записавшийся клиент. */
    const Client& client() const;

    /** @return Дата приёма. */
    const Date& date() const;

    /** @return Интервал приёма. */
    const TimeSlot& slot() const;

    /** @return Цель визита. */
    const std::string& purpose() const;

    /** @return true, если запись отменена. */
    bool isCancelled() const;

    /**
     * @brief Переносит запись на другое время.
     * @param newDate Новая дата.
     * @param newSlot Новый интервал.
     * @throws BusinessRuleException Если запись отменена.
     */
    void reschedule(const Date& newDate, const TimeSlot& newSlot);

    /**
     * @brief Отменяет запись.
     * @throws BusinessRuleException Если запись уже отменена.
     */
    void cancel();

    /**
     * @brief Проверяет конфликт с другой записью.
     * @param other Запись для сравнения.
     * @return true, если обе записи активны, на одну дату и пересекаются.
     */
    bool conflictsWith(const Appointment& other) const;

    /** @return Строка записи для журнала приёма. */
    std::string describe() const;

private:
    std::shared_ptr<Client> client_;
    Date date_;
    TimeSlot slot_;
    std::string purpose_;
    bool cancelled_;
};

}
