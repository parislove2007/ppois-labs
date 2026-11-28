#include "registry/EmployeeRegistry.h"

#include <numeric>
#include <algorithm>

#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace repair
{

EmployeeRegistry::EmployeeRegistry() {}

std::size_t EmployeeRegistry::technicianCount() const { return technicians_.size(); }

std::size_t EmployeeRegistry::managerCount() const { return managers_.size(); }

void EmployeeRegistry::hire(const std::shared_ptr<Technician>& technician)
{
    if (!technician)
    {
        throw ValidationException("Принимаемый мастер не задан");
    }
    technicians_.push_back(technician);
}

void EmployeeRegistry::hire(const std::shared_ptr<Manager>& manager)
{
    if (!manager)
    {
        throw ValidationException("Принимаемый менеджер не задан");
    }
    managers_.push_back(manager);
}

std::shared_ptr<Technician> EmployeeRegistry::technicianAt(std::size_t index) const
{
    if (index >= technicians_.size())
    {
        throw NotFoundException("Мастер с таким номером отсутствует в реестре");
    }
    return technicians_[index];
}

std::shared_ptr<Manager> EmployeeRegistry::managerAt(std::size_t index) const
{
    if (index >= managers_.size())
    {
        throw NotFoundException("Менеджер с таким номером отсутствует в реестре");
    }
    return managers_[index];
}

std::shared_ptr<Technician> EmployeeRegistry::findAvailableFor(
    const DeviceCategory& category) const
{
    const auto position = std::find_if(technicians_.begin(), technicians_.end(),
                                       [&category](const std::shared_ptr<Technician>& master)
                                       {
                                           return master->canServe(category)
                                                  && !master->isOverloaded();
                                       });
    if (position == technicians_.end())
    {
        throw NotFoundException("Нет свободного мастера по категории «" + category.name() + "»");
    }
    return *position;
}

std::shared_ptr<Manager> EmployeeRegistry::findApproverFor(const Money& amount) const
{
    const auto position = std::find_if(managers_.begin(), managers_.end(),
                                       [&amount](const std::shared_ptr<Manager>& manager)
                                       { return manager->canApprove(amount); });
    if (position == managers_.end())
    {
        throw NotFoundException("Нет менеджера с достаточным лимитом утверждения");
    }
    return *position;
}

Money EmployeeRegistry::monthlyPayroll() const
{
    const Money technicianPart =
        std::accumulate(technicians_.begin(), technicians_.end(), Money(),
                        [](const Money& total, const std::shared_ptr<Technician>& master)
                        { return total + master->monthlySalary(); });
    return std::accumulate(managers_.begin(), managers_.end(), technicianPart,
                           [](const Money& total, const std::shared_ptr<Manager>& manager)
                           { return total + manager->monthlySalary(); });
}

}
