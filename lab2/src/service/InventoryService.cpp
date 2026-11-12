#include "service/InventoryService.h"

#include <vector>

#include "error/ValidationException.h"

namespace
{
const int kMinReorderQuantity = 1;
}

namespace repair
{

InventoryService::InventoryService() {}

bool InventoryService::canSupply(const Warehouse& warehouse, const Part& part,
                                 int quantity) const
{
    if (!warehouse.contains(part))
    {
        return false;
    }
    return warehouse.availableQuantity(part) >= quantity;
}

PartReservation InventoryService::reserveForOrder(Warehouse& warehouse, const Part& part,
                                                  int quantity, int orderNumber,
                                                  const Date& createdOn) const
{
    warehouse.reserve(part, quantity);
    return PartReservation(part, quantity, orderNumber, createdOn);
}

void InventoryService::consume(Warehouse& warehouse, const PartUsage& usage) const
{
    warehouse.writeOff(usage.part(), usage.quantity());
}

void InventoryService::cancelReservation(Warehouse& warehouse,
                                         PartReservation& reservation) const
{
    reservation.release();
    warehouse.releaseReservation(reservation.part(), reservation.quantity());
}

SupplyOrder InventoryService::buildReplenishment(const Warehouse& warehouse,
                                                 const Supplier& supplier, int minimum,
                                                 int reorderQuantity, int orderNumber,
                                                 const Date& createdOn) const
{
    if (reorderQuantity < kMinReorderQuantity)
    {
        throw ValidationException("Количество дозаказа должно быть положительным");
    }
    SupplyOrder order(orderNumber, supplier, createdOn);
    const std::vector<Part> needed = warehouse.partsToReorder(minimum);
    for (const Part& part : needed)
    {
        order.addItem(SupplyOrderItem(part, reorderQuantity, part.price()));
    }
    return order;
}

}
