#include "catalog/PriceList.h"

#include <algorithm>

#include "error/NotFoundException.h"

namespace repair
{

PriceList::PriceList() {}

std::size_t PriceList::size() const { return entries_.size(); }

bool PriceList::isEmpty() const { return entries_.empty(); }

void PriceList::setPrice(const ServiceType& serviceType, const Money& price)
{
    const auto position = std::find_if(entries_.begin(), entries_.end(),
                                       [&serviceType](const PriceListEntry& entry)
                                       { return entry.describes(serviceType); });
    if (position != entries_.end())
    {
        entries_.erase(position);
    }
    entries_.push_back(PriceListEntry(serviceType, price));
}

bool PriceList::contains(const ServiceType& serviceType) const
{
    return std::any_of(entries_.begin(), entries_.end(),
                       [&serviceType](const PriceListEntry& entry)
                       { return entry.describes(serviceType); });
}

Money PriceList::priceFor(const ServiceType& serviceType) const
{
    const auto position = std::find_if(entries_.begin(), entries_.end(),
                                       [&serviceType](const PriceListEntry& entry)
                                       { return entry.describes(serviceType); });
    if (position == entries_.end())
    {
        throw NotFoundException("Цена не задана для вида работы: " + serviceType.name());
    }
    return position->price();
}

Money PriceList::totalFor(const ServiceType& serviceType, int repetitions) const
{
    const PriceListEntry entry(serviceType, priceFor(serviceType));
    return entry.totalFor(repetitions);
}

const PriceListEntry& PriceList::entryAt(std::size_t index) const
{
    if (index >= entries_.size())
    {
        throw NotFoundException("Строка прайса с таким номером отсутствует");
    }
    return entries_[index];
}

}
