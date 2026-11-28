#include "value/EmailAddress.h"

#include <algorithm>
#include <ostream>

#include "error/ValidationException.h"

namespace
{
const char kAtSign = '@';
const char kDot = '.';
const std::size_t kSingleOccurrence = 1;

std::size_t countAtSigns(const std::string& value)
{
    return static_cast<std::size_t>(std::count(value.begin(), value.end(), kAtSign));
}
}

namespace repair
{

EmailAddress::EmailAddress(const std::string& value) : value_(value)
{
    if (!isValid(value))
    {
        throw ValidationException("Некорректный адрес электронной почты: " + value);
    }
}

bool EmailAddress::isValid(const std::string& value)
{
    if (countAtSigns(value) != kSingleOccurrence)
    {
        return false;
    }
    const std::size_t atPosition = value.find(kAtSign);
    const bool hasLocalPart = atPosition > 0;
    const bool hasDomain = atPosition + 1 < value.size();
    if (!hasLocalPart || !hasDomain)
    {
        return false;
    }
    return value.find(kDot, atPosition) != std::string::npos;
}

const std::string& EmailAddress::value() const { return value_; }

std::string EmailAddress::localPart() const { return value_.substr(0, value_.find(kAtSign)); }

std::string EmailAddress::domain() const { return value_.substr(value_.find(kAtSign) + 1); }

bool EmailAddress::belongsToDomain(const std::string& expectedDomain) const
{
    return domain() == expectedDomain;
}

bool EmailAddress::operator==(const EmailAddress& other) const { return value_ == other.value_; }

std::ostream& operator<<(std::ostream& out, const EmailAddress& address)
{
    out << address.value();
    return out;
}

}
