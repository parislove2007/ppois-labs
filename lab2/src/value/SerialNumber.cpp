#include "value/SerialNumber.h"

#include <algorithm>
#include <cctype>
#include <ostream>

#include "error/InvalidSerialNumberException.h"
#include "error/ValidationException.h"

namespace
{
const std::size_t kMinLength = 4;
const std::size_t kMaxLength = 24;
const char kHyphen = '-';

std::string toUpperCase(const std::string& rawSerial)
{
    std::string upper;
    for (const char symbol : rawSerial)
    {
        upper += static_cast<char>(std::toupper(static_cast<unsigned char>(symbol)));
    }
    return upper;
}

bool isAllowedSymbol(char symbol)
{
    const unsigned char code = static_cast<unsigned char>(symbol);
    return std::isalnum(code) != 0 || symbol == kHyphen;
}
}

namespace repair
{

SerialNumber::SerialNumber(const std::string& rawSerial) : value_(toUpperCase(rawSerial))
{
    if (!isValid(rawSerial))
    {
        throw InvalidSerialNumberException(rawSerial);
    }
}

bool SerialNumber::isValid(const std::string& rawSerial)
{
    if (rawSerial.size() < kMinLength || rawSerial.size() > kMaxLength)
    {
        return false;
    }
    return std::all_of(rawSerial.begin(), rawSerial.end(), isAllowedSymbol);
}

const std::string& SerialNumber::value() const { return value_; }

std::string SerialNumber::batchPrefix(std::size_t length) const
{
    if (length > value_.size())
    {
        throw ValidationException("Длина префикса превышает длину серийного номера");
    }
    return value_.substr(0, length);
}

bool SerialNumber::operator==(const SerialNumber& other) const { return value_ == other.value_; }

bool SerialNumber::operator<(const SerialNumber& other) const { return value_ < other.value_; }

std::ostream& operator<<(std::ostream& out, const SerialNumber& serial)
{
    out << serial.value();
    return out;
}

}
