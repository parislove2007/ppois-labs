#include "finance/InvoiceLine.h"

#include "error/ValidationException.h"

namespace repair
{

InvoiceLine::InvoiceLine(const std::string& description, const Money& amount)
    : description_(description), amount_(amount)
{
    if (description.empty())
    {
        throw ValidationException("Наименование позиции счёта обязательно");
    }
    if (amount.isZero())
    {
        throw ValidationException("Сумма позиции счёта не может быть нулевой");
    }
}

const std::string& InvoiceLine::description() const { return description_; }

const Money& InvoiceLine::amount() const { return amount_; }

bool InvoiceLine::exceeds(const Money& threshold) const { return threshold < amount_; }

std::string InvoiceLine::describe() const
{
    return description_ + " — " + amount_.toString();
}

}
