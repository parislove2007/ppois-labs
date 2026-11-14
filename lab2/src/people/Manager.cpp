#include "people/Manager.h"

#include "error/BusinessRuleException.h"

namespace
{
const int kNoSubordinates = 0;
const long long kBonusPerSubordinateKopecks = 2000;
const std::string kPositionName = "Менеджер";
}

namespace repair
{

Manager::Manager(const PersonName& name, const PhoneNumber& phone, const Date& hireDate,
                 const Money& baseSalary, const Money& approvalLimit)
    : Employee(name, phone, hireDate, baseSalary),
      approvalLimit_(approvalLimit), subordinateCount_(kNoSubordinates) {}

std::string Manager::position() const { return kPositionName; }

Money Manager::monthlySalary() const
{
    const Money bonus(kBonusPerSubordinateKopecks * subordinateCount_);
    return baseSalary() + bonus;
}

const Money& Manager::approvalLimit() const { return approvalLimit_; }

int Manager::subordinateCount() const { return subordinateCount_; }

bool Manager::canApprove(const Money& amount) const { return !(approvalLimit_ < amount); }

void Manager::addSubordinate() { ++subordinateCount_; }

void Manager::removeSubordinate()
{
    if (subordinateCount_ == kNoSubordinates)
    {
        throw BusinessRuleException("У менеджера нет подчинённых мастеров");
    }
    --subordinateCount_;
}

}
