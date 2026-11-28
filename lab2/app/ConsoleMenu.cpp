#include "ConsoleMenu.h"

#include <istream>
#include <ostream>
#include <stdexcept>

#include "error/RepairException.h"
#include "error/ValidationException.h"
#include "people/CorporateClient.h"
#include "service/InventoryService.h"
#include "service/OrderService.h"
#include "service/PricingService.h"
#include "service/WarrantyService.h"

namespace
{
const int kExitChoice = 0;
const int kDemoWarrantyMonths = 6;
const int kDemoPartQuantity = 2;
const int kDemoStock = 10;
const std::string kDemoLocation = "A-1-3";
const std::string kPrompt = "Выбор: ";
}

namespace repair
{
namespace
{
DeviceModel demoModel()
{
    return DeviceModel(Manufacturer("Bosch", "Германия"),
                       DeviceCategory("Стиральные машины", kDemoWarrantyMonths),
                       "WAN2427", 2020);
}
}
}

namespace repair
{

ConsoleMenu::ConsoleMenu(std::istream& input, std::ostream& output)
    : input_(input), output_(output)
{
    seedReferenceData();
}

void ConsoleMenu::seedReferenceData()
{
    const DeviceCategory washers("Стиральные машины", kDemoWarrantyMonths);
    const DeviceModel model = demoModel();
    const Part bearing("BRG-6204", "Подшипник барабана", Money::fromRubles(25, 0));

    catalog_.add(bearing);
    catalog_.declareCompatibility(bearing.article(), model);
    warehouse_.receive(bearing, kDemoStock, kDemoLocation);
    prices_.setPrice(ServiceType("Замена подшипника", Duration::fromHours(2, 0), true),
                     Money::fromRubles(60, 0));

    auto master = std::make_shared<Technician>(PersonName("Сидоров", "Иван", "Петрович"),
                                               PhoneNumber("375291111111"), Date(2020, 1, 1),
                                               Money::fromRubles(900, 0), 4);
    master->addSpecialization(washers);
    employees_.hire(master);
}

void ConsoleMenu::run()
{
    while (true)
    {
        showMainMenu();
        const std::string line = readLine(kPrompt);
        if (line.empty())
        {
            return;
        }
        try
        {
            const int choice = parseChoice(line);
            if (choice == kExitChoice)
            {
                return;
            }
            dispatch(choice);
        }
        catch (const std::exception& error)
        {
            output_ << "Ошибка: " << error.what() << "\n";
        }
    }
}

void ConsoleMenu::showMainMenu()
{
    output_ << "\n--- Сервисный центр по ремонту бытовой техники ---\n"
            << "1. Зарегистрировать клиента\n"
            << "2. Список клиентов\n"
            << "3. Принять устройство в ремонт\n"
            << "4. Список заказов\n"
            << "5. Остатки на складе\n"
            << "6. Демонстрационный сценарий ремонта\n"
            << "0. Выход\n";
}

void ConsoleMenu::dispatch(int choice)
{
    switch (choice)
    {
        case 1: registerClient(); break;
        case 2: showClients(); break;
        case 3: acceptDevice(); break;
        case 4: showOrders(); break;
        case 5: showWarehouse(); break;
        case 6: runDemoScenario(); break;
        default: output_ << "Неизвестный пункт меню\n"; break;
    }
}

void ConsoleMenu::registerClient()
{
    const std::string surname = readLine("Фамилия: ");
    const std::string givenName = readLine("Имя: ");
    const std::string phone = readLine("Телефон: ");
    const std::string email = readLine("Электронная почта: ");
    const std::string city = readLine("Город: ");
    const std::string street = readLine("Улица: ");
    const std::string house = readLine("Дом: ");

    clients_.registerClient(std::make_shared<Client>(
        PersonName(surname, givenName), PhoneNumber(phone), EmailAddress(email),
        Address(city, street, house)));
    output_ << "Клиент зарегистрирован\n";
}

void ConsoleMenu::showClients()
{
    if (clients_.size() == 0)
    {
        output_ << "Клиентов пока нет\n";
        return;
    }
    for (std::size_t index = 0; index < clients_.size(); ++index)
    {
        const std::shared_ptr<Client> client = clients_.clientAt(index);
        output_ << index + 1 << ". " << client->contactLine()
                << ", " << client->clientKind()
                << ", скидка " << client->loyaltyDiscount() << "\n";
    }
}

void ConsoleMenu::acceptDevice()
{
    const std::shared_ptr<Client> client = firstClientOrThrow();
    const std::string serial = readLine("Серийный номер устройства: ");
    const std::string problem = readLine("Жалоба клиента: ");
    const IntakeAct act(Device(demoModel(), SerialNumber(serial), Date(2022, 5, 1)), problem,
                        Date(2026, 1, 10), "Приёмщик");
    const OrderService service;
    const std::shared_ptr<RepairOrder> order =
        service.openOrder(orders_, client, act, Date(2026, 1, 10));
    output_ << "Открыт заказ номер " << order->number() << "\n";
}

void ConsoleMenu::showOrders()
{
    if (orders_.size() == 0)
    {
        output_ << "Заказов пока нет\n";
        return;
    }
    for (std::size_t index = 0; index < orders_.size(); ++index)
    {
        const std::shared_ptr<RepairOrder> order = orders_.orderAt(index);
        output_ << "Заказ " << order->number() << ": " << toString(order->status())
                << ", клиент " << order->client().name().shortName()
                << ", сумма " << order->totalCost() << "\n";
    }
}

void ConsoleMenu::showWarehouse()
{
    for (std::size_t index = 0; index < warehouse_.itemCount(); ++index)
    {
        const StockItem& item = warehouse_.itemAt(index);
        output_ << item.part() << ": всего " << item.quantity()
                << ", в резерве " << item.reservedQuantity()
                << ", место " << item.location() << "\n";
    }
}

std::shared_ptr<Client> ConsoleMenu::firstClientOrThrow() const
{
    if (clients_.size() == 0)
    {
        throw ValidationException("Сначала зарегистрируйте клиента (пункт 1)");
    }
    return clients_.clientAt(0);
}

std::shared_ptr<RepairOrder> ConsoleMenu::demoOpenOrder()
{
    const std::shared_ptr<Client> client = firstClientOrThrow();
    const IntakeAct act(Device(demoModel(), SerialNumber("WM-2024-0001"), Date(2022, 5, 1)),
                        "Не отжимает бельё", Date(2026, 1, 10), "Приёмщик");
    const OrderService service;
    const std::shared_ptr<RepairOrder> order =
        service.openOrder(orders_, client, act, Date(2026, 1, 10));
    service.assignBestTechnician(*order, employees_);
    output_ << "1. Открыт заказ " << order->number() << ", мастер "
            << order->technician().name().shortName() << "\n";
    return order;
}

void ConsoleMenu::demoDiagnoseAndEstimate(RepairOrder& order)
{
    Diagnostics diagnostics("Сидоров И. П. ", Date(2026, 1, 11), "Износ подшипника барабана");
    diagnostics.recordDefect(Defect("Износ подшипника", "Барабан", 4));
    order.recordDiagnostics(diagnostics, Date(2026, 1, 11));
    output_ << "2. Диагностика: " << order.diagnostics().conclusion() << "\n";

    const ServiceType replacement("Замена подшипника", Duration::fromHours(2, 0), true);
    order.addWorkToEstimate(WorkItem(replacement, "Сидоров И. П. ", Duration::fromHours(2, 0),
                                     prices_.priceFor(replacement)));
    const Part& bearing = catalog_.findByArticle("BRG-6204");
    order.addPartToEstimate(PartUsage(bearing, kDemoPartQuantity, bearing.price()));
    output_ << "3. Смета составлена на сумму " << order.totalCost() << "\n";
}

void ConsoleMenu::demoRepair(RepairOrder& order)
{
    order.approveEstimate(Date(2026, 1, 12));
    output_ << "4. Клиент утвердил смету\n";

    const InventoryService inventory;
    const Part& bearing = catalog_.findByArticle("BRG-6204");
    PartReservation reservation = inventory.reserveForOrder(
        warehouse_, bearing, kDemoPartQuantity, order.number(), Date(2026, 1, 12));
    output_ << "5. Зарезервировано: " << reservation.describe() << "\n";

    order.startRepair(Date(2026, 1, 13));
    inventory.consume(warehouse_, PartUsage(bearing, kDemoPartQuantity, bearing.price()));
    const WarrantyService warranty;
    order.completeRepair(Date(2026, 1, 14), warranty.warrantyMonthsFor(order));
    output_ << "6. Ремонт завершён, гарантия до "
            << warranty.coverageEnd(order) << "\n";
}

Invoice ConsoleMenu::demoSettle(const RepairOrder& order)
{
    const OrderService service;
    const PricingService pricing(prices_);
    Invoice invoice = service.issueInvoice(order, order.number(), Date(2026, 1, 14));
    output_ << "7. Счёт " << invoice.number() << ": к оплате " << invoice.total()
            << " (по расчёту сервиса " << service.amountDue(order, pricing) << ")\n";

    invoice.registerPayment(Payment(invoice.balanceDue(), PaymentMethod::Card,
                                    Date(2026, 1, 15), "ТРМ-000123"));
    output_ << "8. Оплачено, остаток " << invoice.balanceDue() << "\n";
    return invoice;
}

void ConsoleMenu::demoIssue(RepairOrder& order, Invoice& invoice)
{
    const OrderService service;
    if (!service.readyForIssue(order, invoice))
    {
        throw ValidationException("Заказ не готов к выдаче");
    }
    order.issueToClient(Date(2026, 1, 15));
    output_ << "9. Устройство выдано, состояние заказа: " << toString(order.status()) << "\n";
    output_ << "   Завершённых заказов у клиента: " << order.client().completedOrders()
            << ", остаток подшипников на складе: "
            << warehouse_.availableQuantity(catalog_.findByArticle("BRG-6204")) << "\n";
}

void ConsoleMenu::runDemoScenario()
{
    const std::shared_ptr<RepairOrder> order = demoOpenOrder();
    demoDiagnoseAndEstimate(*order);
    demoRepair(*order);
    Invoice invoice = demoSettle(*order);
    demoIssue(*order, invoice);
}

std::string ConsoleMenu::readLine(const std::string& prompt)
{
    output_ << prompt;
    std::string line;
    std::getline(input_, line);
    return line;
}

int ConsoleMenu::parseChoice(const std::string& line)
{
    try
    {
        return std::stoi(line);
    }
    catch (const std::exception&)
    {
        throw ValidationException("Ожидался номер пункта меню, введено: " + line);
    }
}

int ConsoleMenu::readInt(const std::string& prompt)
{
    return parseChoice(readLine(prompt));
}

}
