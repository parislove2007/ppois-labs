#include "value/Percentage.h"

#include <ostream>
#include <string>

#include "error/ValidationException.h"

namespace
{
const int kMinPercent = 0;
const int kMaxPercent = 100;
}

namespace repair
{

Percentage::Percentage() : value_(kMinPercent) {}

Percentage::Percentage(int value) : value_(value)
{
    if (value < kMinPercent || value > kMaxPercent)
    {
        throw ValidationException("Процент вне диапазона 0..100: " + std::to_string(value));
    }
}

int Percentage::value() const { return value_; }

bool Percentage::isZero() const { return value_ == kMinPercent; }

Money Percentage::applyTo(const Money& amount) const
{
    const long long share = amount.kopecks() * value_ / kMaxPercent;
    return Money(share);
}

Money Percentage::remainderOf(const Money& amount) const
{
    return amount - applyTo(amount);
}

Percentage Percentage::combinedWith(const Percentage& other) const
{
    const int total = value_ + other.value_;
    return Percentage(total > kMaxPercent ? kMaxPercent : total);
}

bool Percentage::operator==(const Percentage& other) const { return value_ == other.value_; }

bool Percentage::operator<(const Percentage& other) const { return value_ < other.value_; }

std::ostream& operator<<(std::ostream& out, const Percentage& percentage)
{
    out << percentage.value() << "%";
    return out;
}

}
