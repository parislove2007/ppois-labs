#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/OutOfStockException.h"
#include "error/PartNotFoundException.h"
#include "error/ValidationException.h"
#include "stock/PartReservation.h"
#include "stock/StockItem.h"
#include "stock/Supplier.h"
#include "stock/SupplyOrder.h"
#include "stock/SupplyOrderItem.h"
#include "stock/Warehouse.h"

using namespace repair;

namespace
{
const int kOrderNumber = 77;
const std::string kLocation = "A-1-3";

Part bearing() { return Part("BRG-6204", "Подшипник", Money::fromRubles(25, 0)); }

Part pump() { return Part("PMP-11", "Насос", Money::fromRubles(40, 0)); }

Supplier sampleSupplier()
{
    return Supplier("ТехноСнаб", PhoneNumber("375291234567"), 5, 4);
}
}

TEST(StockItemTest, SeparatesReservedFromAvailable)
{
    StockItem item(bearing(), 10, kLocation);

    EXPECT_EQ(item.quantity(), 10);
    EXPECT_EQ(item.reservedQuantity(), 0);
    EXPECT_EQ(item.availableQuantity(), 10);
    EXPECT_EQ(item.location(), kLocation);
    EXPECT_TRUE(item.part() == bearing());

    item.reserve(4);
    EXPECT_EQ(item.availableQuantity(), 6);
    EXPECT_EQ(item.quantity(), 10);
}

TEST(StockItemTest, RefusesToReserveMoreThanAvailable)
{
    StockItem item(bearing(), 3, kLocation);

    EXPECT_THROW(item.reserve(4), OutOfStockException);
    EXPECT_THROW(item.reserve(0), ValidationException);
}

TEST(StockItemTest, WritesOffOnlyReserved)
{
    StockItem item(bearing(), 10, kLocation);
    item.reserve(4);
    item.writeOff(3);

    EXPECT_EQ(item.quantity(), 7);
    EXPECT_EQ(item.reservedQuantity(), 1);
    EXPECT_THROW(item.writeOff(5), BusinessRuleException);
}

TEST(StockItemTest, ReleasesReservationAndReceives)
{
    StockItem item(bearing(), 10, kLocation);
    item.reserve(4);
    item.releaseReservation(4);

    EXPECT_EQ(item.availableQuantity(), 10);
    EXPECT_THROW(item.releaseReservation(1), BusinessRuleException);

    item.receive(5);
    EXPECT_EQ(item.quantity(), 15);
    EXPECT_THROW(item.receive(0), ValidationException);
}

TEST(StockItemTest, DetectsLowStockAndValidatesInput)
{
    const StockItem item(bearing(), 2, kLocation);

    EXPECT_TRUE(item.isBelowMinimum(5));
    EXPECT_FALSE(item.isBelowMinimum(2));
    EXPECT_THROW(StockItem(bearing(), -1, kLocation), ValidationException);
    EXPECT_THROW(StockItem(bearing(), 1, ""), ValidationException);
}

TEST(WarehouseTest, CreatesPositionOnFirstReceiptAndAddsLater)
{
    Warehouse warehouse;
    warehouse.receive(bearing(), 10, kLocation);
    warehouse.receive(bearing(), 5, kLocation);

    EXPECT_EQ(warehouse.itemCount(), 1u);
    EXPECT_TRUE(warehouse.contains(bearing()));
    EXPECT_EQ(warehouse.availableQuantity(bearing()), 15);
    EXPECT_EQ(warehouse.itemAt(0).location(), kLocation);
}

TEST(WarehouseTest, ReportsUnknownPart)
{
    Warehouse warehouse;

    EXPECT_FALSE(warehouse.contains(pump()));
    EXPECT_THROW(warehouse.availableQuantity(pump()), PartNotFoundException);
    EXPECT_THROW(warehouse.reserve(pump(), 1), PartNotFoundException);
    EXPECT_THROW(warehouse.releaseReservation(pump(), 1), PartNotFoundException);
    EXPECT_THROW(warehouse.writeOff(pump(), 1), PartNotFoundException);
    EXPECT_THROW(warehouse.itemAt(0), NotFoundException);
}

TEST(WarehouseTest, MovesPartsThroughReserveAndWriteOff)
{
    Warehouse warehouse;
    warehouse.receive(bearing(), 10, kLocation);
    warehouse.reserve(bearing(), 4);

    EXPECT_EQ(warehouse.availableQuantity(bearing()), 6);

    warehouse.writeOff(bearing(), 2);
    EXPECT_EQ(warehouse.itemAt(0).quantity(), 8);

    warehouse.releaseReservation(bearing(), 2);
    EXPECT_EQ(warehouse.availableQuantity(bearing()), 8);
}

TEST(WarehouseTest, ListsPartsToReorder)
{
    Warehouse warehouse;
    warehouse.receive(bearing(), 2, kLocation);
    warehouse.receive(pump(), 20, kLocation);

    const std::vector<Part> needed = warehouse.partsToReorder(5);

    EXPECT_EQ(needed.size(), 1u);
    EXPECT_TRUE(needed.front() == bearing());
}

TEST(PartReservationTest, TracksLifecycle)
{
    PartReservation reservation(bearing(), 2, kOrderNumber, Date(2026, 1, 10));

    EXPECT_TRUE(reservation.isActive());
    EXPECT_TRUE(reservation.belongsToOrder(kOrderNumber));
    EXPECT_FALSE(reservation.belongsToOrder(kOrderNumber + 1));
    EXPECT_EQ(reservation.quantity(), 2);
    EXPECT_EQ(reservation.orderNumber(), kOrderNumber);
    EXPECT_TRUE(reservation.part() == bearing());
    EXPECT_TRUE(reservation.createdOn() == Date(2026, 1, 10));

    reservation.release();
    EXPECT_FALSE(reservation.isActive());
    EXPECT_THROW(reservation.release(), BusinessRuleException);
}

TEST(PartReservationTest, DetectsStaleReservations)
{
    PartReservation fresh(bearing(), 1, kOrderNumber, Date(2026, 1, 10));

    EXPECT_TRUE(fresh.isStale(Date(2026, 2, 10), 7));
    EXPECT_FALSE(fresh.isStale(Date(2026, 1, 12), 7));

    fresh.release();
    EXPECT_FALSE(fresh.isStale(Date(2026, 2, 10), 7));
}

TEST(PartReservationTest, DescribesAndValidates)
{
    const PartReservation reservation(bearing(), 2, kOrderNumber, Date(2026, 1, 10));

    EXPECT_EQ(reservation.describe(), "BRG-6204 x2 под заказ 77 от 10.01.2026, активен");
    EXPECT_THROW(PartReservation(bearing(), 0, kOrderNumber, Date(2026, 1, 10)),
                 ValidationException);
    EXPECT_THROW(PartReservation(bearing(), 1, 0, Date(2026, 1, 10)), ValidationException);
}

TEST(SupplierTest, ComputesDeliveryDateAndReliability)
{
    const Supplier supplier = sampleSupplier();

    EXPECT_TRUE(supplier.expectedDeliveryDate(Date(2026, 1, 10)) == Date(2026, 1, 15));
    EXPECT_TRUE(supplier.isReliable(4));
    EXPECT_FALSE(supplier.isReliable(5));
    EXPECT_EQ(supplier.name(), "ТехноСнаб");
    EXPECT_EQ(supplier.deliveryDays(), 5);
    EXPECT_TRUE(supplier.phone() == PhoneNumber("375291234567"));
}

TEST(SupplierTest, MovesRatingWithinBounds)
{
    Supplier supplier = sampleSupplier();
    supplier.raiseRating();

    EXPECT_EQ(supplier.rating(), 5);
    EXPECT_THROW(supplier.raiseRating(), BusinessRuleException);

    Supplier weak("Мелкий", PhoneNumber("375291234567"), 10, 1);
    EXPECT_THROW(weak.lowerRating(), BusinessRuleException);
    supplier.lowerRating();
    EXPECT_EQ(supplier.rating(), 4);
}

TEST(SupplierTest, PrefersFasterThenMoreReliable)
{
    const Supplier fast("Быстрый", PhoneNumber("375291234567"), 2, 3);
    const Supplier slow("Медленный", PhoneNumber("375291234567"), 9, 5);
    const Supplier sameSpeedBetter("Надёжный", PhoneNumber("375291234567"), 2, 5);

    EXPECT_TRUE(fast.isPreferableTo(slow));
    EXPECT_FALSE(fast.isPreferableTo(sameSpeedBetter));
}

TEST(SupplierTest, ValidatesConstructorArguments)
{
    EXPECT_THROW(Supplier("", PhoneNumber("375291234567"), 1, 3), ValidationException);
    EXPECT_THROW(Supplier("X", PhoneNumber("375291234567"), -1, 3), ValidationException);
    EXPECT_THROW(Supplier("X", PhoneNumber("375291234567"), 1, 0), ValidationException);
    EXPECT_THROW(Supplier("X", PhoneNumber("375291234567"), 1, 6), ValidationException);
}

TEST(SupplyOrderItemTest, CountsCostRevenueAndProfit)
{
    const SupplyOrderItem item(bearing(), 4, Money::fromRubles(15, 0));

    EXPECT_EQ(item.totalCost().rubles(), 60);
    EXPECT_EQ(item.expectedRevenue().rubles(), 100);
    EXPECT_EQ(item.expectedProfit().rubles(), 40);
    EXPECT_EQ(item.quantity(), 4);
    EXPECT_EQ(item.purchasePrice().rubles(), 15);
    EXPECT_TRUE(item.part() == bearing());
    EXPECT_EQ(item.describe(), "BRG-6204 x4, закупка 60.00");
    EXPECT_THROW(SupplyOrderItem(bearing(), 0, Money(1)), ValidationException);
}

TEST(SupplyOrderTest, CollectsItemsAndCountsTotals)
{
    SupplyOrder order(1, sampleSupplier(), Date(2026, 1, 10));
    order.addItem(SupplyOrderItem(bearing(), 4, Money::fromRubles(15, 0)));
    order.addItem(SupplyOrderItem(pump(), 2, Money::fromRubles(30, 0)));

    EXPECT_EQ(order.itemCount(), 2u);
    EXPECT_EQ(order.totalCost().rubles(), 120);
    EXPECT_EQ(order.expectedProfit().rubles(), 60);
    EXPECT_EQ(order.number(), 1);
    EXPECT_EQ(order.supplier().name(), "ТехноСнаб");
    EXPECT_TRUE(order.createdOn() == Date(2026, 1, 10));
    EXPECT_TRUE(order.expectedDate() == Date(2026, 1, 15));
    EXPECT_EQ(order.itemAt(0).quantity(), 4);
    EXPECT_THROW(order.itemAt(5), NotFoundException);
}

TEST(SupplyOrderTest, ReceivesOnceAndStocksWarehouse)
{
    Warehouse warehouse;
    SupplyOrder order(1, sampleSupplier(), Date(2026, 1, 10));
    order.addItem(SupplyOrderItem(bearing(), 4, Money::fromRubles(15, 0)));

    EXPECT_FALSE(order.isReceived());
    EXPECT_THROW(order.wasDeliveredOnTime(), BusinessRuleException);

    order.receive(warehouse, Date(2026, 1, 14), kLocation);

    EXPECT_TRUE(order.isReceived());
    EXPECT_TRUE(order.wasDeliveredOnTime());
    EXPECT_EQ(warehouse.availableQuantity(bearing()), 4);
    EXPECT_THROW(order.receive(warehouse, Date(2026, 1, 15), kLocation),
                 BusinessRuleException);
    EXPECT_THROW(order.addItem(SupplyOrderItem(pump(), 1, Money(1))), BusinessRuleException);
}

TEST(SupplyOrderTest, DetectsLateDeliveryAndRefusesEmptyReceipt)
{
    Warehouse warehouse;
    SupplyOrder late(2, sampleSupplier(), Date(2026, 1, 10));

    EXPECT_THROW(late.receive(warehouse, Date(2026, 1, 20), kLocation), BusinessRuleException);

    late.addItem(SupplyOrderItem(bearing(), 1, Money(100)));
    late.receive(warehouse, Date(2026, 1, 20), kLocation);

    EXPECT_FALSE(late.wasDeliveredOnTime());
    EXPECT_THROW(SupplyOrder(0, sampleSupplier(), Date(2026, 1, 10)), ValidationException);
}
