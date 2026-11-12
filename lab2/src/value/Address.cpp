#include "value/Address.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const std::string kStreetPrefix = "ул. ";
const std::string kHousePrefix = "д. ";
const std::string kApartmentPrefix = "кв. ";
const std::string kSeparator = ", ";
}

namespace repair
{

Address::Address(const std::string& city, const std::string& street, const std::string& house)
    : Address(city, street, house, "") {}

Address::Address(const std::string& city, const std::string& street, const std::string& house,
                 const std::string& apartment)
    : city_(city), street_(street), house_(house), apartment_(apartment)
{
    if (city.empty() || street.empty() || house.empty())
    {
        throw ValidationException("Населённый пункт, улица и номер дома обязательны");
    }
}

const std::string& Address::city() const { return city_; }

const std::string& Address::street() const { return street_; }

const std::string& Address::house() const { return house_; }

const std::string& Address::apartment() const { return apartment_; }

bool Address::isPrivateHouse() const { return apartment_.empty(); }

bool Address::isSameCity(const Address& other) const { return city_ == other.city_; }

std::string Address::oneLine() const
{
    const std::string base = city_ + kSeparator + kStreetPrefix + street_
                             + kSeparator + kHousePrefix + house_;
    if (isPrivateHouse())
    {
        return base;
    }
    return base + kSeparator + kApartmentPrefix + apartment_;
}

bool Address::operator==(const Address& other) const
{
    return city_ == other.city_
           && street_ == other.street_
           && house_ == other.house_
           && apartment_ == other.apartment_;
}

std::ostream& operator<<(std::ostream& out, const Address& address)
{
    out << address.oneLine();
    return out;
}

}
