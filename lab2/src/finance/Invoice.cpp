#include "finance/Invoice.h"

#include <numeric>
#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinNumber = 1;
}

namespace repair
{

Invoice::Invoice(int number, int orderNumber, const Date& issuedOn)
    : number_(number), orderNumber_(orderNumber), issuedOn_(issuedOn)
{
    if (number < kMinNumber || orderNumber < kMinNumber)
    {
        throw ValidationException("Номера счёта и заказа должны быть положительными");
    }
}

int Invoice::number() const { return number_; }

int Invoice::orderNumber() const { return orderNumber_; }

const Date& Invoice::issuedOn() const { return issuedOn_; }

std::size_t Invoice::lineCount() const { return lines_.size(); }

std::size_t Invoice::paymentCount() const { return payments_.size(); }

void Invoice::addLine(const InvoiceLine& line)
{
    if (!payments_.empty())
    {
        throw BusinessRuleException("Нельзя изменить счёт, по которому прошли платежи");
    }
    lines_.push_back(line);
}

void Invoice::applyDiscount(const Discount& discount)
{
    if (!payments_.empty())
    {
        throw BusinessRuleException("Нельзя изменить счёт, по которому прошли платежи");
    }
    discount_ = discount;
}

Money Invoice::subtotal() const
{
    return std::accumulate(lines_.begin(), lines_.end(), Money(),
                           [](const Money& total, const InvoiceLine& line)
                           { return total + line.amount(); });
}

Money Invoice::discountAmount() const
{
    if (!discount_.has_value())
    {
        return Money();
    }
    return discount_.value().amountFor(subtotal());
}

Money Invoice::total() const { return subtotal() - discountAmount(); }

Money Invoice::paidAmount() const
{
    return std::accumulate(payments_.begin(), payments_.end(), Money(),
                           [](const Money& paid, const Payment& payment)
                           { return paid + payment.amount(); });
}

Money Invoice::balanceDue() const { return total() - paidAmount(); }

bool Invoice::isPaid() const { return balanceDue().isZero(); }

void Invoice::registerPayment(const Payment& payment)
{
    if (isPaid())
    {
        throw BusinessRuleException("Счёт уже оплачен полностью");
    }
    if (balanceDue() < payment.amount())
    {
        throw BusinessRuleException("Платёж превышает остаток к оплате: остаток "
                                    + balanceDue().toString());
    }
    payments_.push_back(payment);
}

const InvoiceLine& Invoice::lineAt(std::size_t index) const
{
    if (index >= lines_.size())
    {
        throw NotFoundException("Позиция счёта с таким номером отсутствует");
    }
    return lines_[index];
}

const Payment& Invoice::paymentAt(std::size_t index) const
{
    if (index >= payments_.size())
    {
        throw NotFoundException("Платёж с таким номером отсутствует");
    }
    return payments_[index];
}

}
