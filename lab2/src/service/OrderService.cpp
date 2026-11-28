#include "service/OrderService.h"

#include "error/BusinessRuleException.h"

namespace
{
const std::string kWorksLineName = "Работы по заказу";
const std::string kPartsLineName = "Запчасти";
}

namespace repair
{

OrderService::OrderService() {}

std::shared_ptr<RepairOrder> OrderService::openOrder(OrderBook& book,
                                                     const std::shared_ptr<Client>& client,
                                                     const IntakeAct& intakeAct,
                                                     const Date& openedOn) const
{
    const int number = book.issueNumber();
    const auto order = std::make_shared<RepairOrder>(number, client, intakeAct, openedOn);
    book.add(order);
    return order;
}

void OrderService::assignBestTechnician(RepairOrder& order,
                                        const EmployeeRegistry& registry) const
{
    const DeviceCategory& category = order.intakeAct().device().model().category();
    order.assignTechnician(registry.findAvailableFor(category));
}

Invoice OrderService::issueInvoice(const RepairOrder& order, int invoiceNumber,
                                   const Date& issuedOn) const
{
    if (!order.estimate().isApproved())
    {
        throw BusinessRuleException("Нельзя выставить счёт по неутверждённой смете");
    }
    Invoice invoice(invoiceNumber, order.number(), issuedOn);
    const Money works = order.estimate().worksCost();
    const Money parts = order.estimate().partsCost();
    if (!works.isZero())
    {
        invoice.addLine(InvoiceLine(kWorksLineName, works));
    }
    if (!parts.isZero())
    {
        invoice.addLine(InvoiceLine(kPartsLineName, parts));
    }
    invoice.applyDiscount(Discount(order.client().loyaltyDiscount(),
                                   "Накопительная скидка клиента", Money()));
    return invoice;
}

Money OrderService::amountDue(const RepairOrder& order, const PricingService& pricing) const
{
    return pricing.orderTotal(order);
}

bool OrderService::readyForIssue(const RepairOrder& order, const Invoice& invoice) const
{
    return order.status() == OrderStatus::Completed && invoice.isPaid()
           && invoice.orderNumber() == order.number();
}

}
