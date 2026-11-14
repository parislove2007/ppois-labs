#include "stock/SupplyOrder.h"

#include <numeric>
#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinOrderNumber = 1;
}

namespace repair
{

SupplyOrder::SupplyOrder(int number, const Supplier& supplier, const Date& createdOn)
    : number_(number), supplier_(supplier), createdOn_(createdOn)
{
    if (number < kMinOrderNumber)
    {
        throw ValidationException("Номер заказа поставщику должен быть положительным");
    }
}

int SupplyOrder::number() const { return number_; }

const Supplier& SupplyOrder::supplier() const { return supplier_; }

const Date& SupplyOrder::createdOn() const { return createdOn_; }

std::size_t SupplyOrder::itemCount() const { return items_.size(); }

bool SupplyOrder::isReceived() const { return receivedOn_.has_value(); }

Date SupplyOrder::expectedDate() const { return supplier_.expectedDeliveryDate(createdOn_); }

void SupplyOrder::addItem(const SupplyOrderItem& item)
{
    if (isReceived())
    {
        throw BusinessRuleException("Нельзя изменить состав принятой поставки");
    }
    items_.push_back(item);
}

const SupplyOrderItem& SupplyOrder::itemAt(std::size_t index) const
{
    if (index >= items_.size())
    {
        throw NotFoundException("Строка заказа поставщику с таким номером отсутствует");
    }
    return items_[index];
}

Money SupplyOrder::totalCost() const
{
    return std::accumulate(items_.begin(), items_.end(), Money(),
                           [](const Money& total, const SupplyOrderItem& item)
                           { return total + item.totalCost(); });
}

Money SupplyOrder::expectedProfit() const
{
    return std::accumulate(items_.begin(), items_.end(), Money(),
                           [](const Money& total, const SupplyOrderItem& item)
                           { return total + item.expectedProfit(); });
}

void SupplyOrder::receive(Warehouse& warehouse, const Date& receivedOn,
                          const std::string& location)
{
    if (isReceived())
    {
        throw BusinessRuleException("Поставка по этому заказу уже принята");
    }
    if (items_.empty())
    {
        throw BusinessRuleException("Нельзя принять пустую поставку");
    }
    for (const SupplyOrderItem& item : items_)
    {
        warehouse.receive(item.part(), item.quantity(), location);
    }
    receivedOn_ = receivedOn;
}

bool SupplyOrder::wasDeliveredOnTime() const
{
    if (!isReceived())
    {
        throw BusinessRuleException("Поставка ещё не принята");
    }
    return !receivedOn_.value().isAfter(expectedDate());
}

}
