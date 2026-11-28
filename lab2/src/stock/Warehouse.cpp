#include "stock/Warehouse.h"

#include <algorithm>

#include "error/NotFoundException.h"
#include "error/PartNotFoundException.h"

namespace repair
{

Warehouse::Warehouse() {}

std::size_t Warehouse::itemCount() const { return items_.size(); }

bool Warehouse::contains(const Part& part) const
{
    return std::any_of(items_.begin(), items_.end(),
                       [&part](const StockItem& item) { return item.part() == part; });
}

StockItem& Warehouse::findItem(const Part& part)
{
    const auto position = std::find_if(items_.begin(), items_.end(),
                                       [&part](const StockItem& item)
                                       { return item.part() == part; });
    if (position == items_.end())
    {
        throw PartNotFoundException(part.article());
    }
    return *position;
}

const StockItem& Warehouse::findItem(const Part& part) const
{
    const auto position = std::find_if(items_.begin(), items_.end(),
                                       [&part](const StockItem& item)
                                       { return item.part() == part; });
    if (position == items_.end())
    {
        throw PartNotFoundException(part.article());
    }
    return *position;
}

void Warehouse::receive(const Part& part, int quantity, const std::string& location)
{
    if (!contains(part))
    {
        items_.push_back(StockItem(part, quantity, location));
        return;
    }
    findItem(part).receive(quantity);
}

int Warehouse::availableQuantity(const Part& part) const
{
    return findItem(part).availableQuantity();
}

void Warehouse::reserve(const Part& part, int quantity) { findItem(part).reserve(quantity); }

void Warehouse::releaseReservation(const Part& part, int quantity)
{
    findItem(part).releaseReservation(quantity);
}

void Warehouse::writeOff(const Part& part, int quantity) { findItem(part).writeOff(quantity); }

const StockItem& Warehouse::itemAt(std::size_t index) const
{
    if (index >= items_.size())
    {
        throw NotFoundException("Позиция склада с таким номером отсутствует");
    }
    return items_[index];
}

std::vector<Part> Warehouse::partsToReorder(int minimum) const
{
    std::vector<Part> needed;
    for (const StockItem& item : items_)
    {
        if (item.isBelowMinimum(minimum))
        {
            needed.push_back(item.part());
        }
    }
    return needed;
}

}
