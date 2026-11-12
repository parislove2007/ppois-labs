#include "catalog/PriceListEntry.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kMinRepetitions = 1;
}

namespace repair
{

PriceListEntry::PriceListEntry(const ServiceType& serviceType, const Money& price)
    : serviceType_(serviceType), price_(price) {}

const ServiceType& PriceListEntry::serviceType() const { return serviceType_; }

const Money& PriceListEntry::price() const { return price_; }

Money PriceListEntry::totalFor(int repetitions) const
{
    if (repetitions < kMinRepetitions)
    {
        throw ValidationException("Число повторений работы должно быть положительным");
    }
    return price_ * repetitions;
}

bool PriceListEntry::describes(const ServiceType& candidate) const
{
    return serviceType_ == candidate;
}

std::ostream& operator<<(std::ostream& out, const PriceListEntry& entry)
{
    out << entry.serviceType() << " — " << entry.price();
    return out;
}

}
