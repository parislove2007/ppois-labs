#pragma once

#include <iosfwd>
#include <memory>
#include <string>

#include "catalog/PriceList.h"
#include "finance/Invoice.h"
#include "registry/ClientRegistry.h"
#include "registry/EmployeeRegistry.h"
#include "registry/OrderBook.h"
#include "registry/PartCatalog.h"
#include "repair/RepairOrder.h"
#include "stock/Warehouse.h"

namespace repair
{

/**
 * @brief Консольное меню сервисного центра.
 *
 * Слой интерфейса: не содержит правил предметной области и работает
 * только через публичные методы классов домена. Потоки ввода и вывода
 * передаются извне, поэтому меню не привязано к консоли и может
 * управляться из файла или строкового потока.
 */
class ConsoleMenu
{
public:
    /**
     * @brief Создаёт меню, работающее с заданными потоками.
     * @param input Поток ввода.
     * @param output Поток вывода.
     */
    ConsoleMenu(std::istream& input, std::ostream& output);

    /** @brief Запускает главный цикл меню. */
    void run();

private:
    void seedReferenceData();
    void showMainMenu();
    void dispatch(int choice);

    void registerClient();
    void showClients();
    void acceptDevice();
    void showOrders();
    void showWarehouse();
    void runDemoScenario();

    std::shared_ptr<RepairOrder> demoOpenOrder();
    void demoDiagnoseAndEstimate(RepairOrder& order);
    void demoRepair(RepairOrder& order);
    Invoice demoSettle(const RepairOrder& order);
    void demoIssue(RepairOrder& order, Invoice& invoice);

    std::shared_ptr<Client> firstClientOrThrow() const;
    static int parseChoice(const std::string& line);
    std::string readLine(const std::string& prompt);
    int readInt(const std::string& prompt);

    std::istream& input_;
    std::ostream& output_;
    ClientRegistry clients_;
    EmployeeRegistry employees_;
    OrderBook orders_;
    PartCatalog catalog_;
    Warehouse warehouse_;
    PriceList prices_;
};

}
