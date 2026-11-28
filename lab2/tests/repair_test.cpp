#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "error/BusinessRuleException.h"
#include "error/EstimateNotApprovedException.h"
#include "error/InvalidStatusTransitionException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"
#include "people/CorporateClient.h"
#include "repair/Defect.h"
#include "repair/Device.h"
#include "repair/Diagnostics.h"
#include "repair/Estimate.h"
#include "repair/IntakeAct.h"
#include "repair/OrderHistoryEntry.h"
#include "repair/PartUsage.h"
#include "repair/RepairOrder.h"
#include "repair/Warranty.h"
#include "repair/WarrantyClaim.h"
#include "repair/WorkItem.h"

using namespace repair;

namespace
{
const int kOrderNumber = 101;
const int kWarrantyMonths = 6;
const int kGrade = 4;

DeviceCategory washingMachines() { return DeviceCategory("Стиральные машины", 6); }

DeviceModel sampleModel()
{
    return DeviceModel(Manufacturer("Bosch", "Германия"), washingMachines(), "WAN2427", 2020);
}

Device sampleDevice()
{
    return Device(sampleModel(), SerialNumber("WM-2024-0001"), Date(2022, 5, 1));
}

IntakeAct sampleIntake()
{
    return IntakeAct(sampleDevice(), "Не отжимает бельё", Date(2026, 1, 10), "Петров П. П.");
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
                                               Money(100000), kGrade);
    master->addSpecialization(washingMachines());
    return master;
}

ServiceType bearingReplacement()
{
    return ServiceType("Замена подшипника", Duration::fromHours(2, 0), true);
}

WorkItem sampleWork()
{
    return WorkItem(bearingReplacement(), "Сидоров И. ", Duration::fromHours(2, 0),
                    Money::fromRubles(60, 0));
}

PartUsage samplePartUsage()
{
    return PartUsage(Part("BRG-6204", "Подшипник", Money::fromRubles(25, 0)), 2,
                     Money::fromRubles(25, 0));
}

Diagnostics sampleDiagnostics()
{
    Diagnostics result("Сидоров И. ", Date(2026, 1, 11), "Износ подшипника барабана");
    result.recordDefect(Defect("Износ подшипника", "Барабан", 4));
    return result;
}

RepairOrder openedOrder()
{
    return RepairOrder(kOrderNumber, sampleClient(), sampleIntake(), Date(2026, 1, 10));
}
}

TEST(DefectTest, StoresDescriptionUnitAndSeverity)
{
    const Defect defect("Износ подшипника", "Барабан", 4);

    EXPECT_EQ(defect.description(), "Износ подшипника");
    EXPECT_EQ(defect.unit(), "Барабан");
    EXPECT_EQ(defect.severity(), 4);
    EXPECT_TRUE(defect.isBlocking());

    std::ostringstream out;
    out << defect;
    EXPECT_EQ(out.str(), "Барабан: Износ подшипника (критичность 4)");
}

TEST(DefectTest, ComparesSeverityAndIdentity)
{
    const Defect severe("Износ", "Барабан", 5);
    const Defect light("Царапина", "Корпус", 1);

    EXPECT_TRUE(severe.isMoreSevereThan(light));
    EXPECT_FALSE(light.isBlocking());
    EXPECT_TRUE(severe == Defect("Износ", "Барабан", 2));
}

TEST(DefectTest, RejectsEmptyPartsAndBadSeverity)
{
    EXPECT_THROW(Defect("", "Барабан", 3), ValidationException);
    EXPECT_THROW(Defect("Износ", "", 3), ValidationException);
    EXPECT_THROW(Defect("Износ", "Барабан", 0), ValidationException);
    EXPECT_THROW(Defect("Износ", "Барабан", 6), ValidationException);
}

TEST(OrderStatusTest, TranslatesEveryState)
{
    EXPECT_EQ(toString(OrderStatus::Accepted), "Принят");
    EXPECT_EQ(toString(OrderStatus::Diagnosed), "Продиагностирован");
    EXPECT_EQ(toString(OrderStatus::EstimateApproved), "Смета утверждена");
    EXPECT_EQ(toString(OrderStatus::InRepair), "В ремонте");
    EXPECT_EQ(toString(OrderStatus::Completed), "Готов к выдаче");
    EXPECT_EQ(toString(OrderStatus::Issued), "Выдан");
    EXPECT_EQ(toString(OrderStatus::Cancelled), "Отменён");
}

TEST(OrderHistoryEntryTest, DescribesTransition)
{
    const OrderHistoryEntry entry(OrderStatus::Accepted, OrderStatus::Diagnosed,
                                  Date(2026, 1, 11), "Сидоров И.");

    EXPECT_EQ(entry.fromStatus(), OrderStatus::Accepted);
    EXPECT_EQ(entry.toStatus(), OrderStatus::Diagnosed);
    EXPECT_TRUE(entry.changedOn() == Date(2026, 1, 11));
    EXPECT_EQ(entry.changedBy(), "Сидоров И.");
    EXPECT_TRUE(entry.leadsTo(OrderStatus::Diagnosed));
    EXPECT_FALSE(entry.leadsTo(OrderStatus::Issued));
    EXPECT_EQ(entry.describe(), "11.01.2026: Принят -> Продиагностирован, Сидоров И.");
}

TEST(OrderHistoryEntryTest, RequiresActor)
{
    EXPECT_THROW(OrderHistoryEntry(OrderStatus::Accepted, OrderStatus::Diagnosed,
                                   Date(2026, 1, 11), ""),
                 ValidationException);
}

TEST(WorkItemTest, DetectsOvertime)
{
    const WorkItem onTime = sampleWork();
    const WorkItem late(bearingReplacement(), "Сидоров И. ", Duration::fromHours(3, 30),
                        Money::fromRubles(60, 0));

    EXPECT_TRUE(onTime.isWithinNorm());
    EXPECT_TRUE(onTime.overtime().isZero());
    EXPECT_FALSE(late.isWithinNorm());
    EXPECT_EQ(late.overtime().totalMinutes(), 90);
}

TEST(WorkItemTest, ExposesFieldsAndDescription)
{
    const WorkItem work = sampleWork();

    EXPECT_TRUE(work.serviceType() == bearingReplacement());
    EXPECT_EQ(work.performedBy(), "Сидоров И. ");
    EXPECT_EQ(work.spentTime().totalMinutes(), 120);
    EXPECT_EQ(work.price().rubles(), 60);
    EXPECT_EQ(work.describe(), "Замена подшипника, Сидоров И. , 2 ч 0 мин, 60.00");
}

TEST(WorkItemTest, RequiresPerformer)
{
    EXPECT_THROW(WorkItem(bearingReplacement(), "", Duration(10), Money(1)),
                 ValidationException);
}

TEST(PartUsageTest, CountsTotalCost)
{
    const PartUsage usage = samplePartUsage();

    EXPECT_EQ(usage.quantity(), 2);
    EXPECT_EQ(usage.totalCost().rubles(), 50);
    EXPECT_EQ(usage.unitPrice().rubles(), 25);
    EXPECT_EQ(usage.part().article(), "BRG-6204");
    EXPECT_EQ(usage.describe(), "Подшипник x2, 50.00");
}

TEST(PartUsageTest, DetectsCatalogPriceGrowth)
{
    const Part part("BRG-6204", "Подшипник", Money::fromRubles(30, 0));
    const PartUsage cheaper(part, 1, Money::fromRubles(25, 0));
    const PartUsage sameAsCatalog(part, 1, Money::fromRubles(30, 0));

    EXPECT_TRUE(cheaper.priceRoseSinceUsage());
    EXPECT_FALSE(sameAsCatalog.priceRoseSinceUsage());
}

TEST(PartUsageTest, RejectsNonPositiveQuantity)
{
    EXPECT_THROW(PartUsage(Part("A", "B", Money(1)), 0, Money(1)), ValidationException);
}

TEST(DeviceTest, TracksAccessoriesWithoutDuplicates)
{
    Device device = sampleDevice();
    device.addAccessory("Шланг");
    device.addAccessory("Шланг");

    EXPECT_EQ(device.accessoryCount(), 1u);
    EXPECT_TRUE(device.hasAccessory("Шланг"));
    EXPECT_FALSE(device.hasAccessory("Фильтр"));
    EXPECT_THROW(device.addAccessory(""), ValidationException);
}

TEST(DeviceTest, ChecksFactoryWarrantyAndAge)
{
    const Device device = sampleDevice();

    EXPECT_TRUE(device.isUnderFactoryWarranty(Date(2023, 1, 1), 24));
    EXPECT_FALSE(device.isUnderFactoryWarranty(Date(2026, 1, 1), 24));
    EXPECT_EQ(device.ageInDays(Date(2022, 5, 11)), 10);
    EXPECT_EQ(device.ageInDays(Date(2020, 1, 1)), 0);
    EXPECT_TRUE(device == sampleDevice());
    EXPECT_TRUE(device.model() == sampleModel());
    EXPECT_TRUE(device.purchaseDate() == Date(2022, 5, 1));
    EXPECT_TRUE(device.serialNumber() == SerialNumber("WM-2024-0001"));
}

TEST(IntakeActTest, RecordsExternalDefectsWithoutDuplicates)
{
    IntakeAct act = sampleIntake();
    const Defect scratch("Царапина на крышке", "Корпус", 1);
    act.recordExternalDefect(scratch);
    act.recordExternalDefect(scratch);

    EXPECT_EQ(act.externalDefectCount(), 1u);
    EXPECT_TRUE(act.hasExternalDefect(scratch));
    EXPECT_FALSE(act.hasBlockingDefect());

    act.recordExternalDefect(Defect("Трещина корпуса", "Корпус", 5));
    EXPECT_TRUE(act.hasBlockingDefect());
}

TEST(IntakeActTest, ExposesFieldsAndText)
{
    const IntakeAct act = sampleIntake();

    EXPECT_EQ(act.declaredProblem(), "Не отжимает бельё");
    EXPECT_EQ(act.acceptedBy(), "Петров П. П.");
    EXPECT_TRUE(act.acceptedOn() == Date(2026, 1, 10));
    EXPECT_TRUE(act.device() == sampleDevice());
    EXPECT_NE(act.describe().find("Не отжимает бельё"), std::string::npos);
}

TEST(IntakeActTest, RequiresProblemAndReceiver)
{
    EXPECT_THROW(IntakeAct(sampleDevice(), "", Date(2026, 1, 10), "Петров"),
                 ValidationException);
    EXPECT_THROW(IntakeAct(sampleDevice(), "Шумит", Date(2026, 1, 10), ""),
                 ValidationException);
}

TEST(DiagnosticsTest, CollectsDefectsAndFindsMostSevere)
{
    Diagnostics result = sampleDiagnostics();
    result.recordDefect(Defect("Течь", "Патрубок", 2));
    result.recordDefect(Defect("Течь", "Патрубок", 2));

    EXPECT_EQ(result.defectCount(), 2u);
    EXPECT_TRUE(result.requiresRepair());
    EXPECT_EQ(result.mostSevereDefect().severity(), 4);
    EXPECT_EQ(result.defectAt(1).unit(), "Патрубок");
    EXPECT_THROW(result.defectAt(5), NotFoundException);
}

TEST(DiagnosticsTest, HandlesEmptyResult)
{
    const Diagnostics clean("Сидоров И. ", Date(2026, 1, 11), "Неисправность не подтвердилась");

    EXPECT_FALSE(clean.requiresRepair());
    EXPECT_EQ(clean.complexityFactor(), 1);
    EXPECT_THROW(clean.mostSevereDefect(), NotFoundException);
    EXPECT_EQ(clean.performedBy(), "Сидоров И. ");
    EXPECT_EQ(clean.conclusion(), "Неисправность не подтвердилась");
    EXPECT_TRUE(clean.performedOn() == Date(2026, 1, 11));
    EXPECT_NE(clean.describe().find("не подтвердилась"), std::string::npos);
}

TEST(DiagnosticsTest, ScalesComplexityBySeverity)
{
    EXPECT_EQ(sampleDiagnostics().complexityFactor(), 2);
}

TEST(DiagnosticsTest, RequiresPerformerAndConclusion)
{
    EXPECT_THROW(Diagnostics("", Date(2026, 1, 11), "Вывод"), ValidationException);
    EXPECT_THROW(Diagnostics("Мастер", Date(2026, 1, 11), ""), ValidationException);
}

TEST(EstimateTest, StartsEmptyAndUnapproved)
{
    const Estimate estimate;

    EXPECT_TRUE(estimate.isEmpty());
    EXPECT_FALSE(estimate.isApproved());
    EXPECT_TRUE(estimate.totalCost().isZero());
    EXPECT_THROW(estimate.approvalDate(), EstimateNotApprovedException);
}

TEST(EstimateTest, SumsWorksAndParts)
{
    Estimate estimate;
    estimate.addWork(sampleWork());
    estimate.addPart(samplePartUsage());

    EXPECT_EQ(estimate.workCount(), 1u);
    EXPECT_EQ(estimate.partCount(), 1u);
    EXPECT_EQ(estimate.worksCost().rubles(), 60);
    EXPECT_EQ(estimate.partsCost().rubles(), 50);
    EXPECT_EQ(estimate.totalCost().rubles(), 110);
    EXPECT_EQ(estimate.workAt(0).price().rubles(), 60);
    EXPECT_EQ(estimate.partAt(0).quantity(), 2);
}

TEST(EstimateTest, FreezesAfterApproval)
{
    Estimate estimate;
    estimate.addWork(sampleWork());
    estimate.approve(Date(2026, 1, 12));

    EXPECT_TRUE(estimate.isApproved());
    EXPECT_TRUE(estimate.approvalDate() == Date(2026, 1, 12));
    EXPECT_THROW(estimate.addWork(sampleWork()), BusinessRuleException);
    EXPECT_THROW(estimate.addPart(samplePartUsage()), BusinessRuleException);
    EXPECT_THROW(estimate.approve(Date(2026, 1, 13)), BusinessRuleException);
}

TEST(EstimateTest, RefusesToApproveEmptyAndReportsBadIndex)
{
    Estimate estimate;

    EXPECT_THROW(estimate.approve(Date(2026, 1, 12)), BusinessRuleException);
    EXPECT_THROW(estimate.workAt(0), NotFoundException);
    EXPECT_THROW(estimate.partAt(0), NotFoundException);
}

TEST(WarrantyTest, ComputesExpiryAndRemainingDays)
{
    const Warranty warranty(Date(2026, 1, 20), kWarrantyMonths);

    EXPECT_TRUE(warranty.expiryDate() == Date(2026, 7, 20));
    EXPECT_TRUE(warranty.isActiveOn(Date(2026, 7, 20)));
    EXPECT_FALSE(warranty.isActiveOn(Date(2026, 7, 21)));
    EXPECT_EQ(warranty.daysLeftOn(Date(2026, 7, 10)), 10);
    EXPECT_EQ(warranty.daysLeftOn(Date(2027, 1, 1)), 0);
    EXPECT_EQ(warranty.months(), kWarrantyMonths);
    EXPECT_TRUE(warranty.startDate() == Date(2026, 1, 20));
}

TEST(WarrantyTest, CoversOnlyListedUnits)
{
    Warranty warranty(Date(2026, 1, 20), kWarrantyMonths);
    warranty.addCoveredUnit("Барабан");
    warranty.addCoveredUnit("Барабан");

    EXPECT_EQ(warranty.coveredUnitCount(), 1u);
    EXPECT_TRUE(warranty.covers("Барабан"));
    EXPECT_FALSE(warranty.covers("Насос"));
    EXPECT_THROW(warranty.addCoveredUnit(""), ValidationException);
}

TEST(WarrantyTest, RejectsNonPositiveTerm)
{
    EXPECT_THROW(Warranty(Date(2026, 1, 20), 0), ValidationException);
}

TEST(WarrantyClaimTest, MatchesActiveWarrantyForCoveredUnit)
{
    Warranty warranty(Date(2026, 1, 20), kWarrantyMonths);
    warranty.addCoveredUnit("Барабан");
    const WarrantyClaim covered(kOrderNumber, "Барабан", Date(2026, 3, 1));
    const WarrantyClaim otherUnit(kOrderNumber, "Насос", Date(2026, 3, 1));
    const WarrantyClaim tooLate(kOrderNumber, "Барабан", Date(2027, 3, 1));

    EXPECT_TRUE(covered.matches(warranty));
    EXPECT_FALSE(otherUnit.matches(warranty));
    EXPECT_FALSE(tooLate.matches(warranty));
}

TEST(WarrantyClaimTest, ResolvesOnlyOnce)
{
    WarrantyClaim claim(kOrderNumber, "Барабан", Date(2026, 3, 1));

    EXPECT_FALSE(claim.isResolved());
    claim.accept();
    EXPECT_TRUE(claim.isResolved());
    EXPECT_TRUE(claim.isAccepted());
    EXPECT_THROW(claim.accept(), BusinessRuleException);
    EXPECT_THROW(claim.reject("поздно"), BusinessRuleException);
    EXPECT_THROW(claim.rejectionReason(), BusinessRuleException);
}

TEST(WarrantyClaimTest, KeepsRejectionReason)
{
    WarrantyClaim claim(kOrderNumber, "Насос", Date(2026, 3, 1));
    claim.reject("Узел не покрыт гарантией");

    EXPECT_TRUE(claim.isResolved());
    EXPECT_FALSE(claim.isAccepted());
    EXPECT_EQ(claim.rejectionReason(), "Узел не покрыт гарантией");
    EXPECT_EQ(claim.originalOrderNumber(), kOrderNumber);
    EXPECT_EQ(claim.failedUnit(), "Насос");
    EXPECT_TRUE(claim.submittedOn() == Date(2026, 3, 1));
}

TEST(WarrantyClaimTest, ValidatesInput)
{
    EXPECT_THROW(WarrantyClaim(kOrderNumber, "", Date(2026, 3, 1)), ValidationException);
    EXPECT_THROW(WarrantyClaim(0, "Барабан", Date(2026, 3, 1)), ValidationException);

    WarrantyClaim claim(kOrderNumber, "Насос", Date(2026, 3, 1));
    EXPECT_THROW(claim.reject(""), ValidationException);
}

TEST(RepairOrderTest, OpensInAcceptedState)
{
    const RepairOrder order = openedOrder();

    EXPECT_EQ(order.number(), kOrderNumber);
    EXPECT_EQ(order.status(), OrderStatus::Accepted);
    EXPECT_FALSE(order.isClosed());
    EXPECT_FALSE(order.hasTechnician());
    EXPECT_EQ(order.historySize(), 0u);
    EXPECT_TRUE(order.openedOn() == Date(2026, 1, 10));
    EXPECT_TRUE(order.totalCost().isZero());
    EXPECT_EQ(order.client().clientKind(), "Частный клиент");
    EXPECT_EQ(order.intakeAct().declaredProblem(), "Не отжимает бельё");
}

TEST(RepairOrderTest, ValidatesConstructorArguments)
{
    EXPECT_THROW(RepairOrder(0, sampleClient(), sampleIntake(), Date(2026, 1, 10)),
                 ValidationException);
    EXPECT_THROW(RepairOrder(kOrderNumber, nullptr, sampleIntake(), Date(2026, 1, 10)),
                 ValidationException);
}

TEST(RepairOrderTest, AssignsOnlyTechnicianWithMatchingSpecialisation)
{
    RepairOrder order = openedOrder();
    auto wrongMaster = std::make_shared<Technician>(PersonName("Петров", "Олег"),
                                                    PhoneNumber("375292222222"),
                                                    Date(2021, 1, 1), Money(100000), 3);
    wrongMaster->addSpecialization(DeviceCategory("Ноутбуки", 12));

    EXPECT_THROW(order.assignTechnician(wrongMaster), BusinessRuleException);

    auto master = sampleTechnician();
    order.assignTechnician(master);

    EXPECT_TRUE(order.hasTechnician());
    EXPECT_EQ(master->activeOrders(), 1);
    EXPECT_THROW(order.assignTechnician(sampleTechnician()), BusinessRuleException);
}

TEST(RepairOrderTest, ReportsMissingTechnicianAndDiagnostics)
{
    const RepairOrder order = openedOrder();

    EXPECT_THROW(order.technician(), NotFoundException);
    EXPECT_THROW(order.diagnostics(), NotFoundException);
    EXPECT_THROW(order.warranty(), NotFoundException);
}

TEST(RepairOrderTest, RunsFullLifecycle)
{
    RepairOrder order = openedOrder();
    auto master = sampleTechnician();
    order.assignTechnician(master);

    order.recordDiagnostics(sampleDiagnostics(), Date(2026, 1, 11));
    EXPECT_EQ(order.status(), OrderStatus::Diagnosed);
    EXPECT_EQ(order.diagnostics().defectCount(), 1u);

    order.addWorkToEstimate(sampleWork());
    order.addPartToEstimate(samplePartUsage());
    EXPECT_EQ(order.totalCost().rubles(), 110);

    order.approveEstimate(Date(2026, 1, 12));
    EXPECT_EQ(order.status(), OrderStatus::EstimateApproved);
    EXPECT_TRUE(order.estimate().isApproved());

    order.startRepair(Date(2026, 1, 13));
    EXPECT_EQ(order.status(), OrderStatus::InRepair);

    order.completeRepair(Date(2026, 1, 14), kWarrantyMonths);
    EXPECT_EQ(order.status(), OrderStatus::Completed);
    EXPECT_TRUE(order.warranty().covers("Барабан"));

    order.issueToClient(Date(2026, 1, 15));
    EXPECT_EQ(order.status(), OrderStatus::Issued);
    EXPECT_TRUE(order.isClosed());
    EXPECT_EQ(order.client().completedOrders(), 1);
    EXPECT_EQ(master->activeOrders(), 0);
    EXPECT_EQ(order.historySize(), 5u);
    EXPECT_TRUE(order.historyAt(0).leadsTo(OrderStatus::Diagnosed));
    EXPECT_THROW(order.historyAt(99), NotFoundException);
}

TEST(RepairOrderTest, ForbidsSkippingStates)
{
    RepairOrder order = openedOrder();

    EXPECT_THROW(order.approveEstimate(Date(2026, 1, 12)), InvalidStatusTransitionException);
    EXPECT_THROW(order.startRepair(Date(2026, 1, 13)), BusinessRuleException);
    EXPECT_THROW(order.completeRepair(Date(2026, 1, 14), kWarrantyMonths),
                 InvalidStatusTransitionException);
    EXPECT_THROW(order.issueToClient(Date(2026, 1, 15)), InvalidStatusTransitionException);
    EXPECT_THROW(order.addWorkToEstimate(sampleWork()), InvalidStatusTransitionException);
    EXPECT_THROW(order.addPartToEstimate(samplePartUsage()), InvalidStatusTransitionException);
}

TEST(RepairOrderTest, RefusesRepairWithoutTechnician)
{
    RepairOrder order = openedOrder();
    order.recordDiagnostics(sampleDiagnostics(), Date(2026, 1, 11));
    order.addWorkToEstimate(sampleWork());
    order.approveEstimate(Date(2026, 1, 12));

    EXPECT_THROW(order.startRepair(Date(2026, 1, 13)), BusinessRuleException);
}

TEST(RepairOrderTest, CancelsAndReleasesTechnician)
{
    RepairOrder order = openedOrder();
    auto master = sampleTechnician();
    order.assignTechnician(master);
    order.cancel(Date(2026, 1, 11), "Клиент передумал");

    EXPECT_EQ(order.status(), OrderStatus::Cancelled);
    EXPECT_TRUE(order.isClosed());
    EXPECT_EQ(master->activeOrders(), 0);
    EXPECT_THROW(order.assignTechnician(sampleTechnician()), BusinessRuleException);
    EXPECT_THROW(order.cancel(Date(2026, 1, 12), "ещё раз"), InvalidStatusTransitionException);
}

TEST(RepairOrderTest, RequiresCancellationReason)
{
    RepairOrder order = openedOrder();

    EXPECT_THROW(order.cancel(Date(2026, 1, 11), ""), ValidationException);
}

TEST(RepairOrderTest, KeepsCorporateClientType)
{
    const auto company = std::make_shared<CorporateClient>(
        PersonName("Иванов", "Пётр"), PhoneNumber("375291234567"),
        EmailAddress("buh@firma.by"), Address("Минск", "Лесная", "7"),
        "ООО Ромашка", "Д-15", Percentage(7));
    const RepairOrder order(kOrderNumber, company, sampleIntake(), Date(2026, 1, 10));

    EXPECT_EQ(order.client().clientKind(), "Организация по договору");
    EXPECT_EQ(order.client().loyaltyDiscount().value(), 7);
}
