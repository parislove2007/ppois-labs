#include "value/Timestamp.h"

#include <ostream>

namespace fs
{

Timestamp Timestamp::of(std::int64_t seconds)
{
    return Timestamp(seconds);
}

Timestamp::Timestamp() : seconds_(0)
{
}

Timestamp::Timestamp(std::int64_t seconds) : seconds_(seconds)
{
}

std::int64_t Timestamp::seconds() const
{
    return seconds_;
}

Timestamp Timestamp::plusSeconds(std::int64_t delta) const
{
    return Timestamp(seconds_ + delta);
}

std::int64_t Timestamp::diffSeconds(const Timestamp& other) const
{
    return seconds_ - other.seconds_;
}

bool Timestamp::isBefore(const Timestamp& other) const
{
    return seconds_ < other.seconds_;
}

bool Timestamp::isAfter(const Timestamp& other) const
{
    return seconds_ > other.seconds_;
}

bool Timestamp::operator==(const Timestamp& other) const
{
    return seconds_ == other.seconds_;
}

std::ostream& operator<<(std::ostream& out, const Timestamp& timestamp)
{
    return out << timestamp.seconds() << "s";
}

}
