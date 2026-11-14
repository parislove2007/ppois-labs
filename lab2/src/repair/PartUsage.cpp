#include "repair/PartUsage.h"

#include "error/ValidationException.h"

namespace
{
const int kMinQuantity = 1;
}

namespace repair
{

PartUsage::PartUsage(const Part& part, int quantity, const Money& unitPrice)
    : part_(part), quantity_(quantity), unitPrice_(unitPrice)
{
    if (quantity < kMinQuantity)
    {
        throw ValidationException("Количество списываемых запчастей должно быть положительным");
    }
}

const Part& PartUsage::part() const { return part_; }

int PartUsage::quantity() const { return quantity_; }

const Money& PartUsage::unitPrice() const { return unitPrice_; }

Money PartUsage::totalCost() const { return unitPrice_ * quantity_; }

bool PartUsage::priceRoseSinceUsage() const { return unitPrice_ < part_.price(); }

std::string PartUsage::describe() const
{
    return part_.name() + " x" + std::to_string(quantity_) + ", " + totalCost().toString();
}

}
