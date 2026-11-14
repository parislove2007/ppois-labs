#include "stock/SupplyOrderItem.h"

#include "error/ValidationException.h"

namespace
{
const int kMinQuantity = 1;
}

namespace repair
{

SupplyOrderItem::SupplyOrderItem(const Part& part, int quantity, const Money& purchasePrice)
    : part_(part), quantity_(quantity), purchasePrice_(purchasePrice)
{
    if (quantity < kMinQuantity)
    {
        throw ValidationException("Заказываемое количество должно быть положительным");
    }
}

const Part& SupplyOrderItem::part() const { return part_; }

int SupplyOrderItem::quantity() const { return quantity_; }

const Money& SupplyOrderItem::purchasePrice() const { return purchasePrice_; }

Money SupplyOrderItem::totalCost() const { return purchasePrice_ * quantity_; }

Money SupplyOrderItem::expectedRevenue() const { return part_.totalPriceFor(quantity_); }

Money SupplyOrderItem::expectedProfit() const { return expectedRevenue() - totalCost(); }

std::string SupplyOrderItem::describe() const
{
    return part_.article() + " x" + std::to_string(quantity_) + ", закупка "
           + totalCost().toString();
}

}
