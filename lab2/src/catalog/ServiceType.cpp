#include "catalog/ServiceType.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kMinRepetitions = 1;
}

namespace repair
{

ServiceType::ServiceType(const std::string& name, const Duration& normativeDuration,
                         bool requiresDiagnostics)
    : name_(name), normativeDuration_(normativeDuration),
      requiresDiagnostics_(requiresDiagnostics)
{
    if (name.empty())
    {
        throw ValidationException("Название вида работы обязательно");
    }
}

const std::string& ServiceType::name() const { return name_; }

const Duration& ServiceType::normativeDuration() const { return normativeDuration_; }

bool ServiceType::requiresDiagnostics() const { return requiresDiagnostics_; }

Duration ServiceType::estimatedDurationFor(int repetitions) const
{
    if (repetitions < kMinRepetitions)
    {
        throw ValidationException("Число повторений работы должно быть положительным");
    }
    return Duration(normativeDuration_.totalMinutes() * repetitions);
}

bool ServiceType::matchesName(const std::string& candidate) const { return name_ == candidate; }

bool ServiceType::operator==(const ServiceType& other) const { return name_ == other.name_; }

std::ostream& operator<<(std::ostream& out, const ServiceType& serviceType)
{
    out << serviceType.name();
    return out;
}

}
