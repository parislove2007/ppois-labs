#include "stock/PartReservation.h"

#include "error/BusinessRuleException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinQuantity = 1;
const int kMinOrderNumber = 1;
}

namespace repair
{

PartReservation::PartReservation(const Part& part, int quantity, int orderNumber,
                                 const Date& createdOn)
    : part_(part), quantity_(quantity), orderNumber_(orderNumber),
      createdOn_(createdOn), active_(true)
{
    if (quantity < kMinQuantity)
    {
        throw ValidationException("Резервируемое количество должно быть положительным");
    }
    if (orderNumber < kMinOrderNumber)
    {
        throw ValidationException("Номер заказа для резерва должен быть положительным");
    }
}

const Part& PartReservation::part() const { return part_; }

int PartReservation::quantity() const { return quantity_; }

int PartReservation::orderNumber() const { return orderNumber_; }

const Date& PartReservation::createdOn() const { return createdOn_; }

bool PartReservation::isActive() const { return active_; }

void PartReservation::release()
{
    if (!active_)
    {
        throw BusinessRuleException("Резерв уже снят");
    }
    active_ = false;
}

bool PartReservation::belongsToOrder(int candidateOrderNumber) const
{
    return orderNumber_ == candidateOrderNumber;
}

bool PartReservation::isStale(const Date& today, int allowedDays) const
{
    return active_ && createdOn_.daysUntil(today) > allowedDays;
}

std::string PartReservation::describe() const
{
    const std::string state = active_ ? "активен" : "снят";
    return part_.article() + " x" + std::to_string(quantity_) + " под заказ "
           + std::to_string(orderNumber_) + " от " + createdOn_.toString() + ", " + state;
}

}
