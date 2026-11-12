#include "finance/Discount.h"

#include "error/ValidationException.h"

namespace repair
{

Discount::Discount(const Percentage& percentage, const std::string& reason,
                   const Money& minimumAmount)
    : percentage_(percentage), reason_(reason), minimumAmount_(minimumAmount)
{
    if (reason.empty())
    {
        throw ValidationException("Основание для скидки обязательно");
    }
}

const Percentage& Discount::percentage() const { return percentage_; }

const std::string& Discount::reason() const { return reason_; }

const Money& Discount::minimumAmount() const { return minimumAmount_; }

bool Discount::appliesTo(const Money& amount) const { return !(amount < minimumAmount_); }

Money Discount::amountFor(const Money& amount) const
{
    if (!appliesTo(amount))
    {
        return Money();
    }
    return percentage_.applyTo(amount);
}

Money Discount::applyTo(const Money& amount) const { return amount - amountFor(amount); }

bool Discount::isBetterThan(const Discount& other, const Money& amount) const
{
    return other.amountFor(amount) < amountFor(amount);
}

}
