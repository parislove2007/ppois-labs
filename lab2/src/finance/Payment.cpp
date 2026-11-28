#include "finance/Payment.h"

#include "error/ValidationException.h"

namespace repair
{

std::string toString(PaymentMethod method)
{
    switch (method)
    {
        case PaymentMethod::Cash: return "Наличные";
        case PaymentMethod::Card: return "Банковская карта";
        case PaymentMethod::Transfer: return "Безналичный перевод";
    }
    return "Неизвестный способ оплаты";
}

Payment::Payment(const Money& amount, PaymentMethod method, const Date& receivedOn,
                 const std::string& reference)
    : amount_(amount), method_(method), receivedOn_(receivedOn), reference_(reference)
{
    if (amount.isZero())
    {
        throw ValidationException("Сумма платежа не может быть нулевой");
    }
    if (reference.empty())
    {
        throw ValidationException("Номер чека или платёжного поручения обязателен");
    }
}

const Money& Payment::amount() const { return amount_; }

PaymentMethod Payment::method() const { return method_; }

const Date& Payment::receivedOn() const { return receivedOn_; }

const std::string& Payment::reference() const { return reference_; }

bool Payment::isCashless() const { return method_ != PaymentMethod::Cash; }

bool Payment::isOnTime(const Date& dueDate) const { return !receivedOn_.isAfter(dueDate); }

std::string Payment::describe() const
{
    return receivedOn_.toString() + ", " + toString(method_) + ", " + amount_.toString()
           + ", документ " + reference_;
}

}
