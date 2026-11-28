#include "repair/IntakeAct.h"

#include <algorithm>

#include "error/ValidationException.h"

namespace repair
{

IntakeAct::IntakeAct(const Device& device, const std::string& declaredProblem,
                     const Date& acceptedOn, const std::string& acceptedBy)
    : device_(device), declaredProblem_(declaredProblem),
      acceptedOn_(acceptedOn), acceptedBy_(acceptedBy)
{
    if (declaredProblem.empty() || acceptedBy.empty())
    {
        throw ValidationException("Жалоба клиента и принимающий сотрудник обязательны");
    }
}

const Device& IntakeAct::device() const { return device_; }

const std::string& IntakeAct::declaredProblem() const { return declaredProblem_; }

const Date& IntakeAct::acceptedOn() const { return acceptedOn_; }

const std::string& IntakeAct::acceptedBy() const { return acceptedBy_; }

std::size_t IntakeAct::externalDefectCount() const { return externalDefects_.size(); }

void IntakeAct::recordExternalDefect(const Defect& defect)
{
    if (hasExternalDefect(defect))
    {
        return;
    }
    externalDefects_.push_back(defect);
}

bool IntakeAct::hasExternalDefect(const Defect& defect) const
{
    return std::find(externalDefects_.begin(), externalDefects_.end(), defect)
           != externalDefects_.end();
}

bool IntakeAct::hasBlockingDefect() const
{
    return std::any_of(externalDefects_.begin(), externalDefects_.end(),
                       [](const Defect& defect) { return defect.isBlocking(); });
}

std::string IntakeAct::describe() const
{
    return "Акт приёмки от " + acceptedOn_.toString() + ", принял " + acceptedBy_
           + ". Устройство: " + device_.model().fullName()
           + ", серийный номер " + device_.serialNumber().value()
           + ". Жалоба: " + declaredProblem_
           + ". Внешних повреждений: " + std::to_string(externalDefects_.size());
}

}
