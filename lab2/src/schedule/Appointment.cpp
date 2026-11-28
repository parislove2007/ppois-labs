#include "schedule/Appointment.h"

#include "error/BusinessRuleException.h"
#include "error/ValidationException.h"

namespace repair
{

Appointment::Appointment(const std::shared_ptr<Client>& client, const Date& date,
                         const TimeSlot& slot, const std::string& purpose)
    : client_(client), date_(date), slot_(slot), purpose_(purpose), cancelled_(false)
{
    if (!client)
    {
        throw ValidationException("Клиент записи на приём не задан");
    }
    if (purpose.empty())
    {
        throw ValidationException("Цель визита обязательна");
    }
}

const Client& Appointment::client() const { return *client_; }

const Date& Appointment::date() const { return date_; }

const TimeSlot& Appointment::slot() const { return slot_; }

const std::string& Appointment::purpose() const { return purpose_; }

bool Appointment::isCancelled() const { return cancelled_; }

void Appointment::reschedule(const Date& newDate, const TimeSlot& newSlot)
{
    if (cancelled_)
    {
        throw BusinessRuleException("Нельзя перенести отменённую запись");
    }
    date_ = newDate;
    slot_ = newSlot;
}

void Appointment::cancel()
{
    if (cancelled_)
    {
        throw BusinessRuleException("Запись уже отменена");
    }
    cancelled_ = true;
}

bool Appointment::conflictsWith(const Appointment& other) const
{
    if (cancelled_ || other.cancelled_)
    {
        return false;
    }
    return date_ == other.date_ && slot_.overlaps(other.slot_);
}

std::string Appointment::describe() const
{
    const std::string state = cancelled_ ? ", отменена" : "";
    return date_.toString() + " " + slot_.toString() + ", " + client_->name().shortName()
           + "— " + purpose_ + state;
}

}
