#include "value/Money.h"

#include <cstddef>
#include <ostream>

#include "error/InvalidMoneyAmountException.h"

namespace
{
const long long kKopecksPerRuble = 100;
const int kMaxKopecksPart = 99;
const int kMinKopecksPart = 0;
const std::size_t kKopecksDigits = 2;
}

namespace repair
{

Money::Money() : kopecks_(0) {}

Money::Money(long long kopecks) : kopecks_(kopecks)
{
    if (kopecks < 0)
    {
        throw InvalidMoneyAmountException(kopecks);
    }
}

Money Money::fromRubles(long long rubles, int kopecks)
{
    if (kopecks < kMinKopecksPart || kopecks > kMaxKopecksPart)
    {
        throw InvalidMoneyAmountException(kopecks);
    }
    if (rubles < 0)
    {
        throw InvalidMoneyAmountException(rubles);
    }
    return Money(rubles * kKopecksPerRuble + kopecks);
}

long long Money::kopecks() const { return kopecks_; }

long long Money::rubles() const { return kopecks_ / kKopecksPerRuble; }

int Money::kopecksPart() const { return static_cast<int>(kopecks_ % kKopecksPerRuble); }

bool Money::isZero() const { return kopecks_ == 0; }

Money Money::operator+(const Money& other) const
{
    return Money(kopecks_ + other.kopecks_);
}

Money Money::operator-(const Money& other) const
{
    const long long difference = kopecks_ - other.kopecks_;
    if (difference < 0)
    {
        throw InvalidMoneyAmountException(difference);
    }
    return Money(difference);
}

Money Money::operator*(int quantity) const
{
    if (quantity < 0)
    {
        throw InvalidMoneyAmountException(quantity);
    }
    return Money(kopecks_ * quantity);
}

bool Money::operator==(const Money& other) const { return kopecks_ == other.kopecks_; }

bool Money::operator<(const Money& other) const { return kopecks_ < other.kopecks_; }

std::string Money::toString() const
{
    const std::string fraction = std::to_string(kopecksPart());
    const std::string padded = fraction.size() < kKopecksDigits ? "0" + fraction : fraction;
    return std::to_string(rubles()) + "." + padded;
}

std::ostream& operator<<(std::ostream& out, const Money& money)
{
    out << money.toString() << " руб.";
    return out;
}

}
