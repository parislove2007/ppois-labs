#include "value/Duration.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kMinutesPerHour = 60;
}

namespace repair
{

Duration::Duration() : minutes_(0) {}

Duration::Duration(int minutes) : minutes_(minutes)
{
    if (minutes < 0)
    {
        throw ValidationException("Длительность не может быть отрицательной: "
                                  + std::to_string(minutes));
    }
}

Duration Duration::fromHours(int hours, int minutes)
{
    if (hours < 0 || minutes < 0)
    {
        throw ValidationException("Длительность не может быть отрицательной");
    }
    return Duration(hours * kMinutesPerHour + minutes);
}

int Duration::totalMinutes() const { return minutes_; }

int Duration::hours() const { return minutes_ / kMinutesPerHour; }

int Duration::minutesPart() const { return minutes_ % kMinutesPerHour; }

bool Duration::isZero() const { return minutes_ == 0; }

Duration Duration::operator+(const Duration& other) const
{
    return Duration(minutes_ + other.minutes_);
}

bool Duration::isLongerThan(const Duration& other) const { return minutes_ > other.minutes_; }

bool Duration::operator==(const Duration& other) const { return minutes_ == other.minutes_; }

std::string Duration::toString() const
{
    return std::to_string(hours()) + " ч " + std::to_string(minutesPart()) + " мин";
}

std::ostream& operator<<(std::ostream& out, const Duration& duration)
{
    out << duration.toString();
    return out;
}

}
