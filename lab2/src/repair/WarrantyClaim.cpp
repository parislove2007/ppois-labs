#include "repair/WarrantyClaim.h"

#include "error/BusinessRuleException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinOrderNumber = 1;
}

namespace repair
{

WarrantyClaim::WarrantyClaim(int originalOrderNumber, const std::string& failedUnit,
                             const Date& submittedOn)
    : originalOrderNumber_(originalOrderNumber), failedUnit_(failedUnit),
      submittedOn_(submittedOn), resolved_(false), accepted_(false)
{
    if (failedUnit.empty())
    {
        throw ValidationException("Узел, вышедший из строя, не указан");
    }
    if (originalOrderNumber < kMinOrderNumber)
    {
        throw ValidationException("Номер исходного заказа должен быть положительным");
    }
}

int WarrantyClaim::originalOrderNumber() const { return originalOrderNumber_; }

const std::string& WarrantyClaim::failedUnit() const { return failedUnit_; }

const Date& WarrantyClaim::submittedOn() const { return submittedOn_; }

bool WarrantyClaim::isResolved() const { return resolved_; }

bool WarrantyClaim::isAccepted() const { return accepted_; }

bool WarrantyClaim::matches(const Warranty& warranty) const
{
    return warranty.isActiveOn(submittedOn_) && warranty.covers(failedUnit_);
}

void WarrantyClaim::accept()
{
    if (resolved_)
    {
        throw BusinessRuleException("Решение по гарантийному обращению уже принято");
    }
    resolved_ = true;
    accepted_ = true;
}

void WarrantyClaim::reject(const std::string& reason)
{
    if (resolved_)
    {
        throw BusinessRuleException("Решение по гарантийному обращению уже принято");
    }
    if (reason.empty())
    {
        throw ValidationException("Причина отказа по гарантии обязательна");
    }
    resolved_ = true;
    accepted_ = false;
    rejectionReason_ = reason;
}

const std::string& WarrantyClaim::rejectionReason() const
{
    if (!resolved_ || accepted_)
    {
        throw BusinessRuleException("Обращение не отклонено, причины отказа нет");
    }
    return rejectionReason_;
}

}
