#include "repair/Estimate.h"

#include <numeric>
#include "error/BusinessRuleException.h"
#include "error/EstimateNotApprovedException.h"
#include "error/NotFoundException.h"

namespace
{
const int kUnknownOrderNumber = 0;
}

namespace repair
{

Estimate::Estimate() {}

std::size_t Estimate::workCount() const { return works_.size(); }

std::size_t Estimate::partCount() const { return parts_.size(); }

bool Estimate::isApproved() const { return approvedOn_.has_value(); }

bool Estimate::isEmpty() const { return works_.empty() && parts_.empty(); }

void Estimate::addWork(const WorkItem& work)
{
    if (isApproved())
    {
        throw BusinessRuleException("Нельзя изменить утверждённую смету");
    }
    works_.push_back(work);
}

void Estimate::addPart(const PartUsage& usage)
{
    if (isApproved())
    {
        throw BusinessRuleException("Нельзя изменить утверждённую смету");
    }
    parts_.push_back(usage);
}

Money Estimate::worksCost() const
{
    return std::accumulate(works_.begin(), works_.end(), Money(),
                           [](const Money& total, const WorkItem& work)
                           { return total + work.price(); });
}

Money Estimate::partsCost() const
{
    return std::accumulate(parts_.begin(), parts_.end(), Money(),
                           [](const Money& total, const PartUsage& usage)
                           { return total + usage.totalCost(); });
}

Money Estimate::totalCost() const { return worksCost() + partsCost(); }

void Estimate::approve(const Date& approvedOn)
{
    if (isApproved())
    {
        throw BusinessRuleException("Смета уже утверждена клиентом");
    }
    if (isEmpty())
    {
        throw BusinessRuleException("Нельзя утвердить пустую смету");
    }
    approvedOn_ = approvedOn;
}

const Date& Estimate::approvalDate() const
{
    if (!isApproved())
    {
        throw EstimateNotApprovedException(kUnknownOrderNumber);
    }
    return approvedOn_.value();
}

const WorkItem& Estimate::workAt(std::size_t index) const
{
    if (index >= works_.size())
    {
        throw NotFoundException("Позиция работ с таким номером отсутствует в смете");
    }
    return works_[index];
}

const PartUsage& Estimate::partAt(std::size_t index) const
{
    if (index >= parts_.size())
    {
        throw NotFoundException("Позиция запчастей с таким номером отсутствует в смете");
    }
    return parts_[index];
}

}
