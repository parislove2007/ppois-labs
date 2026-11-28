#include "repair/WorkItem.h"

#include "error/ValidationException.h"

namespace
{
const int kNoOvertime = 0;
}

namespace repair
{

WorkItem::WorkItem(const ServiceType& serviceType, const std::string& performedBy,
                   const Duration& spentTime, const Money& price)
    : serviceType_(serviceType), performedBy_(performedBy),
      spentTime_(spentTime), price_(price)
{
    if (performedBy.empty())
    {
        throw ValidationException("Мастер, выполнивший работу, не указан");
    }
}

const ServiceType& WorkItem::serviceType() const { return serviceType_; }

const std::string& WorkItem::performedBy() const { return performedBy_; }

const Duration& WorkItem::spentTime() const { return spentTime_; }

const Money& WorkItem::price() const { return price_; }

bool WorkItem::isWithinNorm() const
{
    return !spentTime_.isLongerThan(serviceType_.normativeDuration());
}

Duration WorkItem::overtime() const
{
    if (isWithinNorm())
    {
        return Duration(kNoOvertime);
    }
    return Duration(spentTime_.totalMinutes() - serviceType_.normativeDuration().totalMinutes());
}

std::string WorkItem::describe() const
{
    return serviceType_.name() + ", " + performedBy_ + ", " + spentTime_.toString()
           + ", " + price_.toString();
}

}
