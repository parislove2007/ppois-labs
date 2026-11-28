#include "repair/Defect.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kMinSeverity = 1;
const int kMaxSeverity = 5;
const int kBlockingSeverity = 4;
}

namespace repair
{

Defect::Defect(const std::string& description, const std::string& unit, int severity)
    : description_(description), unit_(unit), severity_(severity)
{
    if (description.empty() || unit.empty())
    {
        throw ValidationException("Описание дефекта и узел обязательны");
    }
    if (severity < kMinSeverity || severity > kMaxSeverity)
    {
        throw ValidationException("Критичность дефекта вне диапазона 1..5: "
                                  + std::to_string(severity));
    }
}

const std::string& Defect::description() const { return description_; }

const std::string& Defect::unit() const { return unit_; }

int Defect::severity() const { return severity_; }

bool Defect::isBlocking() const { return severity_ >= kBlockingSeverity; }

bool Defect::isMoreSevereThan(const Defect& other) const { return severity_ > other.severity_; }

bool Defect::operator==(const Defect& other) const
{
    return description_ == other.description_ && unit_ == other.unit_;
}

std::ostream& operator<<(std::ostream& out, const Defect& defect)
{
    out << defect.unit() << ": " << defect.description()
        << " (критичность " << defect.severity() << ")";
    return out;
}

}
