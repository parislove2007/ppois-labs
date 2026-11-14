#include "repair/OrderHistoryEntry.h"

#include "error/ValidationException.h"

namespace repair
{

OrderHistoryEntry::OrderHistoryEntry(OrderStatus fromStatus, OrderStatus toStatus,
                                     const Date& changedOn, const std::string& changedBy)
    : fromStatus_(fromStatus), toStatus_(toStatus), changedOn_(changedOn), changedBy_(changedBy)
{
    if (changedBy.empty())
    {
        throw ValidationException("Сотрудник, изменивший состояние заказа, не указан");
    }
}

OrderStatus OrderHistoryEntry::fromStatus() const { return fromStatus_; }

OrderStatus OrderHistoryEntry::toStatus() const { return toStatus_; }

const Date& OrderHistoryEntry::changedOn() const { return changedOn_; }

const std::string& OrderHistoryEntry::changedBy() const { return changedBy_; }

std::string OrderHistoryEntry::describe() const
{
    return changedOn_.toString() + ": " + toString(fromStatus_) + " -> "
           + toString(toStatus_) + ", " + changedBy_;
}

bool OrderHistoryEntry::leadsTo(OrderStatus status) const { return toStatus_ == status; }

}
