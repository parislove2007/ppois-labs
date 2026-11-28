#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "catalog/DeviceCategory.h"
#include "error/BusinessRuleException.h"
#include "error/ValidationException.h"
#include "people/Client.h"
#include "people/CorporateClient.h"
#include "people/Manager.h"
#include "people/Technician.h"

using namespace repair;

namespace
{
const int kSampleGrade = 4;
const int kMaxActiveOrders = 5;
const long long kBaseSalaryKopecks = 100000;

PersonName sampleName() { return PersonName("Иванов", "Пётр", "Сергеевич"); }

PhoneNumber samplePhone() { return PhoneNumber("375291234567"); }

EmailAddress sampleEmail() { return EmailAddress("client@example.com"); }

Address sampleAddress() { return Address("Минск", "Лесная", "7", "12"); }

Date hireDate() { return Date(2020, 6, 15); }

Technician sampleTechnician()
{
    return Technician(sampleName(), samplePhone(), hireDate(),
                      Money(kBaseSalaryKopecks), kSampleGrade);
}

Manager sampleManager()
{
    return Manager(sampleName(), samplePhone(), hireDate(),
                   Money(kBaseSalaryKopecks), Money::fromRubles(500, 0));
}

Client sampleClient()
{
    return Client(sampleName(), samplePhone(), sampleEmail(), sampleAddress());
}

DeviceCategory laptops() { return DeviceCategory("Ноутбуки", 12); }

DeviceCategory fridges() { return DeviceCategory("Холодильники", 6); }
}

TEST(EmployeeTest, CountsYearsOfServiceBeforeAnniversary)
{
    const Technician master = sampleTechnician();

    EXPECT_EQ(master.yearsOfService(Date(2025, 6, 14)), 4);
    EXPECT_EQ(master.yearsOfService(Date(2025, 6, 15)), 5);
    EXPECT_EQ(master.yearsOfService(Date(2019, 1, 1)), 0);
}

TEST(EmployeeTest, ChecksSeniorityBonusEligibility)
{
    const Technician master = sampleTechnician();

    EXPECT_TRUE(master.isEligibleForSeniorityBonus(Date(2025, 7, 1), 5));
    EXPECT_FALSE(master.isEligibleForSeniorityBonus(Date(2025, 7, 1), 10));
}

TEST(EmployeeTest, ExposesContactDataAndBaseSalary)
{
    const Technician master = sampleTechnician();

    EXPECT_TRUE(master.name() == sampleName());
    EXPECT_TRUE(master.phone() == samplePhone());
    EXPECT_TRUE(master.hireDate() == hireDate());
    EXPECT_EQ(master.baseSalary().kopecks(), kBaseSalaryKopecks);
}

TEST(EmployeeTest, PositionAndSalaryAreResolvedByActualType)
{
    const std::unique_ptr<Employee> master(new Technician(sampleTechnician()));
    const std::unique_ptr<Employee> boss(new Manager(sampleManager()));

    EXPECT_EQ(master->position(), "Мастер по ремонту");
    EXPECT_EQ(boss->position(), "Менеджер");
    EXPECT_GT(master->monthlySalary().kopecks(), master->baseSalary().kopecks());
    EXPECT_EQ(boss->monthlySalary().kopecks(), boss->baseSalary().kopecks());
}

TEST(TechnicianTest, RejectsGradeOutsideRange)
{
    EXPECT_THROW(Technician(sampleName(), samplePhone(), hireDate(), Money(1), 0),
                 ValidationException);
    EXPECT_THROW(Technician(sampleName(), samplePhone(), hireDate(), Money(1), 7),
                 ValidationException);
}

TEST(TechnicianTest, AddsSalaryBonusForGrade)
{
    const Technician master = sampleTechnician();

    EXPECT_EQ(master.grade(), kSampleGrade);
    EXPECT_EQ(master.monthlySalary().kopecks(), 120000);
}

TEST(TechnicianTest, TracksSpecialisationsWithoutDuplicates)
{
    Technician master = sampleTechnician();
    master.addSpecialization(laptops());
    master.addSpecialization(laptops());

    EXPECT_EQ(master.specializationCount(), 1u);
    EXPECT_TRUE(master.canServe(laptops()));
    EXPECT_FALSE(master.canServe(fridges()));
}

TEST(TechnicianTest, LimitsSimultaneousOrders)
{
    Technician master = sampleTechnician();

    EXPECT_EQ(master.activeOrders(), 0);
    for (int taken = 0; taken < kMaxActiveOrders; ++taken)
    {
        master.takeOrder();
    }

    EXPECT_TRUE(master.isOverloaded());
    EXPECT_THROW(master.takeOrder(), BusinessRuleException);
}

TEST(TechnicianTest, ReleasesOrdersAndRefusesToGoBelowZero)
{
    Technician master = sampleTechnician();
    master.takeOrder();
    master.releaseOrder();

    EXPECT_EQ(master.activeOrders(), 0);
    EXPECT_THROW(master.releaseOrder(), BusinessRuleException);
}

TEST(ManagerTest, ApprovesOnlyWithinLimit)
{
    const Manager boss = sampleManager();

    EXPECT_TRUE(boss.canApprove(Money::fromRubles(499, 0)));
    EXPECT_TRUE(boss.canApprove(Money::fromRubles(500, 0)));
    EXPECT_FALSE(boss.canApprove(Money::fromRubles(501, 0)));
    EXPECT_EQ(boss.approvalLimit().rubles(), 500);
}

TEST(ManagerTest, PaysBonusPerSubordinate)
{
    Manager boss = sampleManager();
    boss.addSubordinate();
    boss.addSubordinate();

    EXPECT_EQ(boss.subordinateCount(), 2);
    EXPECT_EQ(boss.monthlySalary().kopecks(), kBaseSalaryKopecks + 4000);
}

TEST(ManagerTest, RefusesToRemoveMissingSubordinate)
{
    Manager boss = sampleManager();

    EXPECT_THROW(boss.removeSubordinate(), BusinessRuleException);
    boss.addSubordinate();
    boss.removeSubordinate();
    EXPECT_EQ(boss.subordinateCount(), 0);
}

TEST(ClientTest, StartsWithoutOrdersAndDiscount)
{
    const Client client = sampleClient();

    EXPECT_EQ(client.completedOrders(), 0);
    EXPECT_TRUE(client.loyaltyDiscount().isZero());
    EXPECT_EQ(client.clientKind(), "Частный клиент");
}

TEST(ClientTest, ExposesContactData)
{
    const Client client = sampleClient();

    EXPECT_TRUE(client.name() == sampleName());
    EXPECT_TRUE(client.phone() == samplePhone());
    EXPECT_TRUE(client.email() == sampleEmail());
    EXPECT_TRUE(client.address() == sampleAddress());
    EXPECT_EQ(client.contactLine(), "Иванов П. С. — +375 (29) 123-45-67");
}

TEST(ClientTest, GrowsDiscountEveryFiveOrdersUpToCap)
{
    Client client = sampleClient();
    for (int order = 0; order < 5; ++order)
    {
        client.registerCompletedOrder();
    }

    EXPECT_EQ(client.loyaltyDiscount().value(), 2);

    for (int order = 0; order < 95; ++order)
    {
        client.registerCompletedOrder();
    }

    EXPECT_EQ(client.loyaltyDiscount().value(), 10);
}

TEST(ClientTest, DetectsRegularCustomer)
{
    Client client = sampleClient();
    client.registerCompletedOrder();

    EXPECT_TRUE(client.isRegular(1));
    EXPECT_FALSE(client.isRegular(2));
}

TEST(CorporateClientTest, RequiresCompanyAndContract)
{
    EXPECT_THROW(CorporateClient(sampleName(), samplePhone(), sampleEmail(), sampleAddress(),
                                 "", "Д-1", Percentage(5)),
                 ValidationException);
    EXPECT_THROW(CorporateClient(sampleName(), samplePhone(), sampleEmail(), sampleAddress(),
                                 "ООО Ромашка", "", Percentage(5)),
                 ValidationException);
}

TEST(CorporateClientTest, UsesContractDiscountWhenItIsLarger)
{
    const CorporateClient company(sampleName(), samplePhone(), sampleEmail(), sampleAddress(),
                                  "ООО Ромашка", "Д-1", Percentage(7));

    EXPECT_EQ(company.loyaltyDiscount().value(), 7);
    EXPECT_EQ(company.clientKind(), "Организация по договору");
    EXPECT_EQ(company.companyName(), "ООО Ромашка");
    EXPECT_EQ(company.contractNumber(), "Д-1");
    EXPECT_EQ(company.contractDiscount().value(), 7);
}

TEST(CorporateClientTest, KeepsAccumulatedDiscountWhenItExceedsContract)
{
    CorporateClient company(sampleName(), samplePhone(), sampleEmail(), sampleAddress(),
                            "ООО Ромашка", "Д-1", Percentage(1));
    for (int order = 0; order < 100; ++order)
    {
        company.registerCompletedOrder();
    }

    EXPECT_EQ(company.loyaltyDiscount().value(), 10);
}

TEST(CorporateClientTest, DiscountIsResolvedByActualType)
{
    const std::unique_ptr<Client> asBase(
        new CorporateClient(sampleName(), samplePhone(), sampleEmail(), sampleAddress(),
                            "ООО Ромашка", "Д-1", Percentage(7)));

    EXPECT_EQ(asBase->loyaltyDiscount().value(), 7);
    EXPECT_EQ(asBase->clientKind(), "Организация по договору");
}
