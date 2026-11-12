#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "error/BusinessRuleException.h"
#include "error/ClientNotFoundException.h"
#include "error/NotFoundException.h"
#include "error/OrderNotFoundException.h"
#include "error/PartNotFoundException.h"
#include "error/ValidationException.h"
#include "people/CorporateClient.h"
#include "registry/ClientRegistry.h"
#include "registry/EmployeeRegistry.h"
#include "registry/OrderBook.h"
#include "registry/PartCatalog.h"

using namespace repair;

namespace
{
const long long kBaseSalaryKopecks = 100000;

DeviceCategory washingMachines() { return DeviceCategory("Стиральные машины", 6); }

DeviceCategory laptops() { return DeviceCategory("Ноутбуки", 12); }

DeviceModel sampleModel()
{
    return DeviceModel(Manufacturer("Bosch", "Германия"), washingMachines(), "WAN2427", 2020);
}

std::shared_ptr<Client> clientWithPhone(const std::string& phone)
{
    return std::make_shared<Client>(PersonName("Иванов", "Пётр"), PhoneNumber(phone),
                                    EmailAddress("client@example.com"),
                                    Address("Минск", "Лесная", "7"));
}

std::shared_ptr<Technician> technicianFor(const DeviceCategory& category)
{
    auto master = std::make_shared<Technician>(PersonName("Сидоров", "Иван"),
                                               PhoneNumber("375291111111"), Date(2020, 1, 1),
                                               Money(kBaseSalaryKopecks), 4);
    master->addSpecialization(category);
    return master;
}

std::shared_ptr<Manager> managerWithLimit(long long rubles)
{
    return std::make_shared<Manager>(PersonName("Петров", "Олег"), PhoneNumber("375292222222"),
                                     Date(2019, 1, 1), Money(kBaseSalaryKopecks),
                                     Money::fromRubles(rubles, 0));
}

std::shared_ptr<RepairOrder> orderNumbered(int number)
{
    const IntakeAct act(Device(sampleModel(), SerialNumber("WM-0001"), Date(2022, 5, 1)),
                        "Шумит", Date(2026, 1, 10), "Петров П. П.");
    return std::make_shared<RepairOrder>(number, clientWithPhone("375291234567"), act,
                                         Date(2026, 1, 10));
}
}

TEST(ClientRegistryTest, RegistersAndFindsByPhone)
{
    ClientRegistry registry;
    const auto client = clientWithPhone("375291234567");
    registry.registerClient(client);

    EXPECT_EQ(registry.size(), 1u);
    EXPECT_TRUE(registry.contains(PhoneNumber("375291234567")));
    EXPECT_EQ(registry.findByPhone(PhoneNumber("375291234567")), client);
    EXPECT_EQ(registry.clientAt(0), client);
}

TEST(ClientRegistryTest, RefusesDuplicatePhoneAndNullClient)
{
    ClientRegistry registry;
    registry.registerClient(clientWithPhone("375291234567"));

    EXPECT_THROW(registry.registerClient(clientWithPhone("375291234567")),
                 BusinessRuleException);
    EXPECT_THROW(registry.registerClient(nullptr), ValidationException);
}

TEST(ClientRegistryTest, ReportsMissingClientAndIndex)
{
    const ClientRegistry registry;

    EXPECT_FALSE(registry.contains(PhoneNumber("375291234567")));
    EXPECT_THROW(registry.findByPhone(PhoneNumber("375291234567")), ClientNotFoundException);
    EXPECT_THROW(registry.clientAt(0), NotFoundException);
}

TEST(ClientRegistryTest, SelectsRegularClients)
{
    ClientRegistry registry;
    const auto regular = clientWithPhone("375291234567");
    const auto newcomer = clientWithPhone("375299999999");
    regular->registerCompletedOrder();
    registry.registerClient(regular);
    registry.registerClient(newcomer);

    const std::vector<std::shared_ptr<Client>> regulars = registry.regularClients(1);

    EXPECT_EQ(regulars.size(), 1u);
    EXPECT_EQ(regulars.front(), regular);
}

TEST(ClientRegistryTest, KeepsCorporateClientType)
{
    ClientRegistry registry;
    registry.registerClient(std::make_shared<CorporateClient>(
        PersonName("Иванов", "Пётр"), PhoneNumber("375291234567"),
        EmailAddress("buh@firma.by"), Address("Минск", "Лесная", "7"),
        "ООО Ромашка", "Д-15", Percentage(7)));

    EXPECT_EQ(registry.clientAt(0)->clientKind(), "Организация по договору");
}

TEST(EmployeeRegistryTest, HiresBothRolesSeparately)
{
    EmployeeRegistry registry;
    registry.hire(technicianFor(washingMachines()));
    registry.hire(managerWithLimit(500));

    EXPECT_EQ(registry.technicianCount(), 1u);
    EXPECT_EQ(registry.managerCount(), 1u);
    EXPECT_EQ(registry.technicianAt(0)->position(), "Мастер по ремонту");
    EXPECT_EQ(registry.managerAt(0)->position(), "Менеджер");
}

TEST(EmployeeRegistryTest, FindsFreeTechnicianBySpecialisation)
{
    EmployeeRegistry registry;
    const auto master = technicianFor(washingMachines());
    registry.hire(master);

    EXPECT_EQ(registry.findAvailableFor(washingMachines()), master);
    EXPECT_THROW(registry.findAvailableFor(laptops()), NotFoundException);
}

TEST(EmployeeRegistryTest, SkipsOverloadedTechnician)
{
    EmployeeRegistry registry;
    const auto master = technicianFor(washingMachines());
    for (int taken = 0; taken < 5; ++taken)
    {
        master->takeOrder();
    }
    registry.hire(master);

    EXPECT_THROW(registry.findAvailableFor(washingMachines()), NotFoundException);
}

TEST(EmployeeRegistryTest, FindsApproverWithSufficientLimit)
{
    EmployeeRegistry registry;
    registry.hire(managerWithLimit(100));

    EXPECT_EQ(registry.findApproverFor(Money::fromRubles(100, 0))->approvalLimit().rubles(),
              100);
    EXPECT_THROW(registry.findApproverFor(Money::fromRubles(101, 0)), NotFoundException);
}

TEST(EmployeeRegistryTest, SumsPayrollAndValidatesInput)
{
    EmployeeRegistry registry;
    registry.hire(technicianFor(washingMachines()));
    registry.hire(managerWithLimit(500));

    EXPECT_EQ(registry.monthlyPayroll().kopecks(), 120000 + 100000);
    EXPECT_THROW(registry.hire(std::shared_ptr<Technician>()), ValidationException);
    EXPECT_THROW(registry.hire(std::shared_ptr<Manager>()), ValidationException);
    EXPECT_THROW(registry.technicianAt(5), NotFoundException);
    EXPECT_THROW(registry.managerAt(5), NotFoundException);
}

TEST(OrderBookTest, IssuesSequentialNumbers)
{
    OrderBook book;

    EXPECT_EQ(book.nextNumber(), 1);
    EXPECT_EQ(book.issueNumber(), 1);
    EXPECT_EQ(book.issueNumber(), 2);
    EXPECT_EQ(book.nextNumber(), 3);
}

TEST(OrderBookTest, StoresAndFindsOrders)
{
    OrderBook book;
    const auto order = orderNumbered(1);
    book.add(order);

    EXPECT_EQ(book.size(), 1u);
    EXPECT_TRUE(book.contains(1));
    EXPECT_EQ(book.findByNumber(1), order);
    EXPECT_EQ(book.orderAt(0), order);
    EXPECT_EQ(book.openOrderCount(), 1u);
}

TEST(OrderBookTest, RefusesDuplicateNumberAndNullOrder)
{
    OrderBook book;
    book.add(orderNumbered(1));

    EXPECT_THROW(book.add(orderNumbered(1)), BusinessRuleException);
    EXPECT_THROW(book.add(nullptr), ValidationException);
}

TEST(OrderBookTest, ReportsMissingOrder)
{
    const OrderBook book;

    EXPECT_FALSE(book.contains(1));
    EXPECT_THROW(book.findByNumber(1), OrderNotFoundException);
    EXPECT_THROW(book.orderAt(0), NotFoundException);
}

TEST(OrderBookTest, FiltersByStatusAndCountsOpen)
{
    OrderBook book;
    const auto first = orderNumbered(1);
    const auto second = orderNumbered(2);
    book.add(first);
    book.add(second);
    second->cancel(Date(2026, 1, 11), "Клиент передумал");

    EXPECT_EQ(book.ordersWithStatus(OrderStatus::Accepted).size(), 1u);
    EXPECT_EQ(book.ordersWithStatus(OrderStatus::Cancelled).size(), 1u);
    EXPECT_EQ(book.openOrderCount(), 1u);
}

TEST(PartCatalogTest, StoresUniqueArticles)
{
    PartCatalog catalog;
    catalog.add(Part("BRG-6204", "Подшипник", Money::fromRubles(25, 0)));

    EXPECT_EQ(catalog.size(), 1u);
    EXPECT_TRUE(catalog.contains("BRG-6204"));
    EXPECT_EQ(catalog.findByArticle("BRG-6204").name(), "Подшипник");
    EXPECT_EQ(catalog.partAt(0).article(), "BRG-6204");
    EXPECT_THROW(catalog.add(Part("BRG-6204", "Другой", Money(1))), BusinessRuleException);
}

TEST(PartCatalogTest, ReportsMissingArticle)
{
    const PartCatalog catalog;

    EXPECT_FALSE(catalog.contains("NONE"));
    EXPECT_THROW(catalog.findByArticle("NONE"), PartNotFoundException);
    EXPECT_THROW(catalog.partAt(0), NotFoundException);
}

TEST(PartCatalogTest, SelectsPartsCompatibleWithModel)
{
    PartCatalog catalog;
    catalog.add(Part("BRG-6204", "Подшипник", Money::fromRubles(25, 0)));
    catalog.add(Part("KEY-99", "Клавиатура", Money::fromRubles(70, 0)));
    catalog.declareCompatibility("BRG-6204", sampleModel());

    const std::vector<Part> compatible = catalog.partsFor(sampleModel());

    EXPECT_EQ(compatible.size(), 1u);
    EXPECT_EQ(compatible.front().article(), "BRG-6204");
    EXPECT_THROW(catalog.declareCompatibility("NONE", sampleModel()), PartNotFoundException);
}
