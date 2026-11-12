#include "catalog/Manufacturer.h"

#include <ostream>

#include "error/ValidationException.h"

namespace repair
{

Manufacturer::Manufacturer(const std::string& name, const std::string& country)
    : name_(name), country_(country)
{
    if (name.empty() || country.empty())
    {
        throw ValidationException("Название марки и страна производителя обязательны");
    }
}

const std::string& Manufacturer::name() const { return name_; }

const std::string& Manufacturer::country() const { return country_; }

bool Manufacturer::isDomesticFor(const std::string& homeCountry) const
{
    return country_ == homeCountry;
}

bool Manufacturer::operator==(const Manufacturer& other) const
{
    return name_ == other.name_ && country_ == other.country_;
}

std::ostream& operator<<(std::ostream& out, const Manufacturer& manufacturer)
{
    out << manufacturer.name() << " (" << manufacturer.country() << ")";
    return out;
}

}
