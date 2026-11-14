#include "repair/Warranty.h"

#include <algorithm>

#include "error/ValidationException.h"

namespace
{
const int kMinMonths = 1;
const int kNoDays = 0;
}

namespace repair
{

Warranty::Warranty(const Date& startDate, int months) : startDate_(startDate), months_(months)
{
    if (months < kMinMonths)
    {
        throw ValidationException("Срок гарантии должен быть положительным: "
                                  + std::to_string(months));
    }
}

const Date& Warranty::startDate() const { return startDate_; }

int Warranty::months() const { return months_; }

std::size_t Warranty::coveredUnitCount() const { return coveredUnits_.size(); }

Date Warranty::expiryDate() const { return startDate_.plusMonths(months_); }

void Warranty::addCoveredUnit(const std::string& unit)
{
    if (unit.empty())
    {
        throw ValidationException("Название покрытого гарантией узла не может быть пустым");
    }
    if (covers(unit))
    {
        return;
    }
    coveredUnits_.push_back(unit);
}

bool Warranty::covers(const std::string& unit) const
{
    return std::find(coveredUnits_.begin(), coveredUnits_.end(), unit) != coveredUnits_.end();
}

bool Warranty::isActiveOn(const Date& date) const { return !date.isAfter(expiryDate()); }

int Warranty::daysLeftOn(const Date& date) const
{
    const int days = date.daysUntil(expiryDate());
    return days < kNoDays ? kNoDays : days;
}

}
