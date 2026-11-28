#include "repair/Diagnostics.h"

#include <algorithm>

#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinComplexityFactor = 1;
const int kSeverityPerComplexityStep = 2;
}

namespace repair
{

Diagnostics::Diagnostics(const std::string& performedBy, const Date& performedOn,
                         const std::string& conclusion)
    : performedBy_(performedBy), performedOn_(performedOn), conclusion_(conclusion)
{
    if (performedBy.empty() || conclusion.empty())
    {
        throw ValidationException("Мастер и заключение диагностики обязательны");
    }
}

const std::string& Diagnostics::performedBy() const { return performedBy_; }

const Date& Diagnostics::performedOn() const { return performedOn_; }

const std::string& Diagnostics::conclusion() const { return conclusion_; }

std::size_t Diagnostics::defectCount() const { return foundDefects_.size(); }

void Diagnostics::recordDefect(const Defect& defect)
{
    const bool alreadyKnown = std::find(foundDefects_.begin(), foundDefects_.end(), defect)
                              != foundDefects_.end();
    if (alreadyKnown)
    {
        return;
    }
    foundDefects_.push_back(defect);
}

bool Diagnostics::requiresRepair() const { return !foundDefects_.empty(); }

const Defect& Diagnostics::mostSevereDefect() const
{
    if (foundDefects_.empty())
    {
        throw NotFoundException("Диагностика не выявила неисправностей");
    }
    return *std::max_element(foundDefects_.begin(), foundDefects_.end(),
                             [](const Defect& left, const Defect& right)
                             { return right.isMoreSevereThan(left); });
}

const Defect& Diagnostics::defectAt(std::size_t index) const
{
    if (index >= foundDefects_.size())
    {
        throw NotFoundException("Неисправность с таким номером не найдена");
    }
    return foundDefects_[index];
}

int Diagnostics::complexityFactor() const
{
    if (!requiresRepair())
    {
        return kMinComplexityFactor;
    }
    const int severity = mostSevereDefect().severity();
    const int factor = severity / kSeverityPerComplexityStep;
    return factor < kMinComplexityFactor ? kMinComplexityFactor : factor;
}

std::string Diagnostics::describe() const
{
    return "Диагностика от " + performedOn_.toString() + ", мастер " + performedBy_
           + ". Найдено неисправностей: " + std::to_string(foundDefects_.size())
           + ". Заключение: " + conclusion_;
}

}
