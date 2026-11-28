#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/OutOfStockException.h"
#include "error/ValidationException.h"
#include "people/CorporateClient.h"
#include "service/InventoryService.h"
#include "service/OrderService.h"
#include "service/PricingService.h"
#include "service/SchedulingService.h"
#include "service/WarrantyService.h"

using namespace repair;

namespace
{
const int kWorkdayMinutes = 480;
const std::string kLocation = "A-1-3";

DeviceCategory washingMachines() { return DeviceCategory("Стиральные машины", 6); }

DeviceModel sampleModel()
{
    return DeviceModel(Manufacturer("Bosch", "Германия"), washingMachines(), "WAN2427", 2020);
}

ServiceType bearingReplacement()
{
    return ServiceType("Замена подшипника", Duration::fromHours(2, 0), true);
}

Part bearing() { return Part("BRG-6204", "Подшипник", Money::fromRubles(25, 0)); }

PriceList samplePriceList()
{
    PriceList prices;
    prices.setPrice(bearingReplacement(), Money::fromRubles(60, 0));
    return prices;
}

std::shared_ptr<Client> sampleClient()
{
    return std::make_shared<Client>(PersonName("Иванов", "Пётр"), PhoneNumber("375291234567"),
                                    EmailAddress("client@example.com"),
                                    Address("Минск", "Лесная", "7"));
}

std::shared_ptr<Technician> sampleTechnician()
{
    auto master = std::make_shared<Technician>(PersonName("Сидоров", "Иван"),
                                               PhoneNumber("375291111111"), Date(2020, 1, 1),
                                               Money(100000), 4);
    master->addSpecialization(washingMachines());
    return master;
}

IntakeAct sampleIntake()
{
    return IntakeAct(Device(sampleModel(), SerialNumber("WM-0001"), Date(2022, 5, 1)),
                     "Не отжимает", Date(2026, 1, 10), "Петров П. П.");
}

Diagnostics sampleDiagnostics()
{
    Diagnostics result("Сидоров И. ", Date(2026, 1, 11), "Износ подшипника");
    result.recordDefect(Defect("Износ подшипника", "Барабан", 4));
    return result;
}

WorkItem sampleWork()
{
    return WorkItem(bearingReplacement(), "Сидоров И. ", Duration::fromHours(2, 0),
                    Money::fromRubles(60, 0));
}

std::shared_ptr<RepairOrder> completedOrder(const std::shared_ptr<Client>& client)
{
    auto order = std::make_shared<RepairOrder>(1, client, sampleIntake(), Date(2026, 1, 10));
    order->assignTechnician(sampleTechnician());
    order->recordDiagnostics(sampleDiagnostics(), Date(2026, 1, 11));
    order->addWorkToEstimate(sampleWork());
    order->addPartToEstimate(PartUsage(bearing(), 2, Money::fromRubles(25, 0)));
    order->approveEstimate(Date(2026, 1, 12));
    order->startRepair(Date(2026, 1, 13));
    order->completeRepair(Date(2026, 1, 14), 6);
    return order;
}
}

TEST(PricingServiceTest, UsesPriceListForWorks)
{
    const PricingService pricing(samplePriceList());

    EXPECT_TRUE(pricing.isPriceKnown(bearingReplacement()));
    EXPECT_FALSE(pricing.isPriceKnown(ServiceType("Чистка", Duration(30), false)));
    EXPECT_EQ(pricing.workCost(bearingReplacement(), 2).rubles(), 120);
    EXPECT_THROW(pricing.workCost(ServiceType("Чистка", Duration(30), false), 1),
                 NotFoundException);
}

TEST(PricingServiceTest, AppliesLoyaltyDiscount)
{
    const PricingService pricing(samplePriceList());
    const auto client = sampleClient();
    for (int order = 0; order < 5; ++order)
    {
        client->registerCompletedOrder();
    }

    EXPECT_EQ(pricing.priceForClient(*client, Money::fromRubles(100, 0)).rubles(), 98);
}

TEST(PricingServiceTest, PicksBetterOfTwoDiscounts)
{
    const PricingService pricing(samplePriceList());
    const auto client = sampleClient();
    const Discount promotion(Percentage(20), "Акция", Money::fromRubles(50, 0));

    EXPECT_EQ(pricing.bestPriceFor(*client, Money::fromRubles(100, 0), promotion).rubles(), 80);

    const Discount tiny(Percentage(1), "Мелочь", Money::fromRubles(1000, 0));
    EXPECT_EQ(pricing.bestPriceFor(*client, Money::fromRubles(100, 0), tiny).rubles(), 100);
}

TEST(PricingServiceTest, CountsOrderTotalWithClientDiscount)
{
    const PricingService pricing(samplePriceList());
    const auto company = std::make_shared<CorporateClient>(
        PersonName("Иванов", "Пётр"), PhoneNumber("375291234567"),
        EmailAddress("buh@firma.by"), Address("Минск", "Лесная", "7"),
        "ООО Ромашка", "Д-15", Percentage(10));

    EXPECT_EQ(pricing.orderTotal(*completedOrder(company)).rubles(), 99);
}

TEST(SchedulingServiceTest, PicksTechnicianThroughRegistry)
{
    const SchedulingService scheduling;
    EmployeeRegistry registry;
    const auto master = sampleTechnician();
    registry.hire(master);

    EXPECT_EQ(scheduling.pickTechnician(registry, washingMachines()), master);
    EXPECT_THROW(scheduling.pickTechnician(registry, DeviceCategory("Ноутбуки", 12)),
                 NotFoundException);
}

TEST(SchedulingServiceTest, PlacesSlotsAndDetectsConflict)
{
    const SchedulingService scheduling;
    WorkSchedule schedule("Сидоров И. ", Date(2026, 1, 20));
    const TimeSlot morning(9, 0, Duration(90));

    EXPECT_TRUE(scheduling.canPlace(schedule, morning));
    scheduling.place(schedule, morning);

    EXPECT_FALSE(scheduling.canPlace(schedule, TimeSlot(10, 0, Duration(60))));
    EXPECT_THROW(scheduling.place(schedule, TimeSlot(10, 0, Duration(60))),
                 BusinessRuleException);
}

TEST(SchedulingServiceTest, SumsPlannedTimeFromEstimate)
{
    const SchedulingService scheduling;
    Estimate estimate;
    estimate.addWork(sampleWork());
    estimate.addWork(sampleWork());

    EXPECT_EQ(scheduling.plannedTime(estimate).totalMinutes(), 240);
    EXPECT_TRUE(scheduling.plannedTime(Estimate()).isZero());
}

TEST(SchedulingServiceTest, EstimatesCompletionDate)
{
    const SchedulingService scheduling;
    const Date start(2026, 1, 20);

    EXPECT_TRUE(scheduling.estimatedCompletion(start, Duration(0), kWorkdayMinutes) == start);
    EXPECT_TRUE(scheduling.estimatedCompletion(start, Duration(480), kWorkdayMinutes) == start);
    EXPECT_TRUE(scheduling.estimatedCompletion(start, Duration(481), kWorkdayMinutes)
                == Date(2026, 1, 21));
    EXPECT_THROW(scheduling.estimatedCompletion(start, Duration(60), 0), ValidationException);
}

TEST(InventoryServiceTest, ChecksAvailabilityBeforeReserving)
{
    const InventoryService inventory;
    Warehouse warehouse;

    EXPECT_FALSE(inventory.canSupply(warehouse, bearing(), 1));

    warehouse.receive(bearing(), 5, kLocation);
    EXPECT_TRUE(inventory.canSupply(warehouse, bearing(), 5));
    EXPECT_FALSE(inventory.canSupply(warehouse, bearing(), 6));
}

TEST(InventoryServiceTest, ReservesConsumesAndCancels)
{
    const InventoryService inventory;
    Warehouse warehouse;
    warehouse.receive(bearing(), 10, kLocation);

    PartReservation reservation =
        inventory.reserveForOrder(warehouse, bearing(), 4, 1, Date(2026, 1, 10));

    EXPECT_EQ(warehouse.availableQuantity(bearing()), 6);
    EXPECT_TRUE(reservation.isActive());

    inventory.consume(warehouse, PartUsage(bearing(), 2, Money::fromRubles(25, 0)));
    EXPECT_EQ(warehouse.itemAt(0).quantity(), 8);

    PartReservation second =
        inventory.reserveForOrder(warehouse, bearing(), 2, 2, Date(2026, 1, 10));
    inventory.cancelReservation(warehouse, second);

    EXPECT_FALSE(second.isActive());
    EXPECT_THROW(inventory.cancelReservation(warehouse, second), BusinessRuleException);
    EXPECT_THROW(inventory.reserveForOrder(warehouse, bearing(), 99, 3, Date(2026, 1, 10)),
                 OutOfStockException);
}

TEST(InventoryServiceTest, BuildsReplenishmentOrderForLowStock)
{
    const InventoryService inventory;
    Warehouse warehouse;
    warehouse.receive(bearing(), 1, kLocation);
    warehouse.receive(Part("PMP-11", "Насос", Money::fromRubles(40, 0)), 50, kLocation);
    const Supplier supplier("ТехноСнаб", PhoneNumber("375291234567"), 5, 4);

    const SupplyOrder order =
        inventory.buildReplenishment(warehouse, supplier, 5, 10, 1, Date(2026, 1, 10));

    EXPECT_EQ(order.itemCount(), 1u);
    EXPECT_EQ(order.itemAt(0).quantity(), 10);
    EXPECT_THROW(inventory.buildReplenishment(warehouse, supplier, 5, 0, 1, Date(2026, 1, 10)),
                 ValidationException);
}

TEST(WarrantyServiceTest, ComputesWarrantyTermFromCategoryAndComplexity)
{
    const WarrantyService warranty;

    EXPECT_EQ(warranty.warrantyMonthsFor(*completedOrder(sampleClient())), 12);
}

TEST(WarrantyServiceTest, AcceptsCoveredClaim)
{
    const WarrantyService warranty;
    const auto order = completedOrder(sampleClient());
    WarrantyClaim claim(order->number(), "Барабан", Date(2026, 3, 1));

    EXPECT_TRUE(warranty.isCoveredCase(*order, claim));
    warranty.process(*order, claim);

    EXPECT_TRUE(claim.isAccepted());
    EXPECT_TRUE(warranty.coverageEnd(*order) == Date(2026, 7, 14));
}

TEST(WarrantyServiceTest, RejectsClaimForUncoveredUnit)
{
    const WarrantyService warranty;
    const auto order = completedOrder(sampleClient());
    WarrantyClaim claim(order->number(), "Насос", Date(2026, 3, 1));

    EXPECT_FALSE(warranty.isCoveredCase(*order, claim));
    warranty.process(*order, claim);

    EXPECT_FALSE(claim.isAccepted());
    EXPECT_TRUE(claim.isResolved());
    EXPECT_NE(claim.rejectionReason().find("не покрыт"), std::string::npos);
}

TEST(OrderServiceTest, OpensOrderWithNumberFromBook)
{
    const OrderService service;
    OrderBook book;

    const auto order = service.openOrder(book, sampleClient(), sampleIntake(),
                                         Date(2026, 1, 10));

    EXPECT_EQ(order->number(), 1);
    EXPECT_EQ(book.size(), 1u);
    EXPECT_EQ(book.findByNumber(1), order);
}

TEST(OrderServiceTest, AssignsTechnicianFromRegistry)
{
    const OrderService service;
    OrderBook book;
    EmployeeRegistry registry;
    const auto master = sampleTechnician();
    registry.hire(master);
    const auto order = service.openOrder(book, sampleClient(), sampleIntake(),
                                         Date(2026, 1, 10));

    service.assignBestTechnician(*order, registry);

    EXPECT_TRUE(order->hasTechnician());
    EXPECT_EQ(master->activeOrders(), 1);
}

TEST(OrderServiceTest, IssuesInvoiceOnlyForApprovedEstimate)
{
    const OrderService service;
    const auto order = completedOrder(sampleClient());

    const Invoice invoice = service.issueInvoice(*order, 500, Date(2026, 1, 14));

    EXPECT_EQ(invoice.lineCount(), 2u);
    EXPECT_EQ(invoice.subtotal().rubles(), 110);
    EXPECT_EQ(invoice.orderNumber(), order->number());

    const auto fresh = std::make_shared<RepairOrder>(2, sampleClient(), sampleIntake(),
                                                     Date(2026, 1, 10));
    EXPECT_THROW(service.issueInvoice(*fresh, 501, Date(2026, 1, 14)), BusinessRuleException);
}

TEST(OrderServiceTest, ChecksReadinessForIssue)
{
    const OrderService service;
    const auto order = completedOrder(sampleClient());
    Invoice invoice = service.issueInvoice(*order, 500, Date(2026, 1, 14));

    EXPECT_FALSE(service.readyForIssue(*order, invoice));

    invoice.registerPayment(Payment(invoice.balanceDue(), PaymentMethod::Card,
                                    Date(2026, 1, 15), "ТРМ-1"));

    EXPECT_TRUE(service.readyForIssue(*order, invoice));
}

TEST(OrderServiceTest, CountsAmountDueThroughPricing)
{
    const OrderService service;
    const PricingService pricing(samplePriceList());
    const auto order = completedOrder(sampleClient());

    EXPECT_EQ(service.amountDue(*order, pricing).rubles(), 110);
}
