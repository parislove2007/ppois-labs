#include "value/PhoneNumber.h"

#include <cctype>
#include <ostream>

#include "error/InvalidPhoneNumberException.h"

namespace
{
const std::string kCountryCode = "375";
const std::size_t kDigitCount = 12;
const std::size_t kOperatorCodeStart = 3;
const std::size_t kOperatorCodeLength = 2;
const std::size_t kSubscriberStart = 5;
const std::size_t kFirstGroupLength = 3;
const std::size_t kSecondGroupStart = 8;
const std::size_t kPairLength = 2;
const std::size_t kThirdGroupStart = 10;

std::string keepDigitsOnly(const std::string& rawNumber)
{
    std::string digits;
    for (const char symbol : rawNumber)
    {
        if (std::isdigit(static_cast<unsigned char>(symbol)) != 0)
        {
            digits += symbol;
        }
    }
    return digits;
}
}

namespace repair
{

PhoneNumber::PhoneNumber(const std::string& rawNumber) : digits_(keepDigitsOnly(rawNumber))
{
    if (!isValid(rawNumber))
    {
        throw InvalidPhoneNumberException(rawNumber);
    }
}

bool PhoneNumber::isValid(const std::string& rawNumber)
{
    const std::string digits = keepDigitsOnly(rawNumber);
    if (digits.size() != kDigitCount)
    {
        return false;
    }
    return digits.compare(0, kCountryCode.size(), kCountryCode) == 0;
}

const std::string& PhoneNumber::digits() const { return digits_; }

std::string PhoneNumber::operatorCode() const
{
    return digits_.substr(kOperatorCodeStart, kOperatorCodeLength);
}

std::string PhoneNumber::formatted() const
{
    const std::string firstGroup = digits_.substr(kSubscriberStart, kFirstGroupLength);
    const std::string secondGroup = digits_.substr(kSecondGroupStart, kPairLength);
    const std::string thirdGroup = digits_.substr(kThirdGroupStart, kPairLength);
    return "+" + kCountryCode + " (" + operatorCode() + ") "
           + firstGroup + "-" + secondGroup + "-" + thirdGroup;
}

bool PhoneNumber::operator==(const PhoneNumber& other) const { return digits_ == other.digits_; }

std::ostream& operator<<(std::ostream& out, const PhoneNumber& number)
{
    out << number.formatted();
    return out;
}

}
