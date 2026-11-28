#include "people/Employee.h"

namespace
{
const int kMonthsPerYear = 12;
}

namespace repair
{

Employee::Employee(const PersonName& name, const PhoneNumber& phone, const Date& hireDate,
                   const Money& baseSalary)
    : name_(name), phone_(phone), hireDate_(hireDate), baseSalary_(baseSalary) {}

Employee::~Employee() {}

const PersonName& Employee::name() const { return name_; }

const PhoneNumber& Employee::phone() const { return phone_; }

const Date& Employee::hireDate() const { return hireDate_; }

const Money& Employee::baseSalary() const { return baseSalary_; }

Money Employee::monthlySalary() const { return baseSalary_; }

int Employee::yearsOfService(const Date& today) const
{
    int years = today.year() - hireDate_.year();
    const bool anniversaryNotReached = today.month() < hireDate_.month()
                                       || (today.month() == hireDate_.month()
                                           && today.day() < hireDate_.day());
    if (anniversaryNotReached)
    {
        --years;
    }
    return years < 0 ? 0 : years;
}

bool Employee::isEligibleForSeniorityBonus(const Date& today, int requiredYears) const
{
    return yearsOfService(today) >= requiredYears;
}

}
