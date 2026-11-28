#include "value/Checksum.h"

#include <cstdint>
#include <ostream>
#include <sstream>

namespace fs
{

namespace
{

/** @brief Считает FNV-1a 64 бит и форматирует его шестнадцатеричной строкой. */
std::string fnv1a(const std::string& content)
{
    const std::uint64_t kOffset = 14695981039346656037ULL;
    const std::uint64_t kPrime = 1099511628211ULL;
    std::uint64_t hash = kOffset;
    for (char ch : content)
    {
        hash ^= static_cast<std::uint8_t>(ch);
        hash *= kPrime;
    }
    std::ostringstream out;
    out << std::hex << hash;
    std::string digits = out.str();
    while (digits.size() < 16)
    {
        digits = "0" + digits;
    }
    return digits;
}

} // namespace

Checksum Checksum::calculate(const std::string& content)
{
    return Checksum(fnv1a(content));
}

Checksum::Checksum() : value_(fnv1a(""))
{
}

Checksum::Checksum(std::string value) : value_(std::move(value))
{
}

const std::string& Checksum::value() const
{
    return value_;
}

bool Checksum::verify(const std::string& content) const
{
    return value_ == fnv1a(content);
}

bool Checksum::operator==(const Checksum& other) const
{
    return value_ == other.value_;
}

std::ostream& operator<<(std::ostream& out, const Checksum& checksum)
{
    return out << checksum.value();
}

}
