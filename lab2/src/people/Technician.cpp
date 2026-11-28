#include "people/Technician.h"

#include <algorithm>

#include "error/BusinessRuleException.h"
#include "error/ValidationException.h"
#include "value/Percentage.h"

namespace
{
const int kMinGrade = 1;
const int kMaxGrade = 6;
const int kBonusPercentPerGrade = 5;
const int kMaxActiveOrders = 5;
const int kNoActiveOrders = 0;
const std::string kPositionName = "Мастер по ремонту";
}

namespace repair
{

Technician::Technician(const PersonName& name, const PhoneNumber& phone, const Date& hireDate,
                       const Money& baseSalary, int grade)
    : Employee(name, phone, hireDate, baseSalary), grade_(grade), activeOrders_(kNoActiveOrders)
{
    if (grade < kMinGrade || grade > kMaxGrade)
    {
        throw ValidationException("Разряд мастера вне диапазона 1..6: "
                                  + std::to_string(grade));
    }
}

std::string Technician::position() const { return kPositionName; }

Money Technician::monthlySalary() const
{
    const Percentage bonus(grade_ * kBonusPercentPerGrade);
    return baseSalary() + bonus.applyTo(baseSalary());
}

int Technician::grade() const { return grade_; }

int Technician::activeOrders() const { return activeOrders_; }

std::size_t Technician::specializationCount() const { return specializations_.size(); }

void Technician::addSpecialization(const DeviceCategory& category)
{
    if (canServe(category))
    {
        return;
    }
    specializations_.push_back(category);
}

bool Technician::canServe(const DeviceCategory& category) const
{
    return std::find(specializations_.begin(), specializations_.end(), category)
           != specializations_.end();
}

bool Technician::isOverloaded() const { return activeOrders_ >= kMaxActiveOrders; }

void Technician::takeOrder()
{
    if (isOverloaded())
    {
        throw BusinessRuleException("Мастер " + name().shortName()
                                    + "уже ведёт максимальное число заказов");
    }
    ++activeOrders_;
}

void Technician::releaseOrder()
{
    if (activeOrders_ == kNoActiveOrders)
    {
        throw BusinessRuleException("У мастера " + name().shortName()
                                    + "нет заказов в работе");
    }
    --activeOrders_;
}

}
