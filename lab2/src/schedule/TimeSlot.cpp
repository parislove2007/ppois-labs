#include "schedule/TimeSlot.h"

#include "error/ValidationException.h"

namespace
{
const int kMinutesPerHour = 60;
const int kHoursPerDay = 24;
const int kMinutesPerDay = kHoursPerDay * kMinutesPerHour;
const std::size_t kTimeDigits = 2;

std::string padded(int number)
{
    std::string text = std::to_string(number);
    while (text.size() < kTimeDigits)
    {
        text = "0" + text;
    }
    return text;
}

std::string formatMinutes(int minutesFromMidnight)
{
    return padded(minutesFromMidnight / kMinutesPerHour) + ":"
           + padded(minutesFromMidnight % kMinutesPerHour);
}
}

namespace repair
{

TimeSlot::TimeSlot(int startHour, int startMinute, const Duration& duration)
    : startMinutes_(startHour * kMinutesPerHour + startMinute), duration_(duration)
{
    const bool hourInRange = startHour >= 0 && startHour < kHoursPerDay;
    const bool minuteInRange = startMinute >= 0 && startMinute < kMinutesPerHour;
    if (!hourInRange || !minuteInRange)
    {
        throw ValidationException("Недопустимое время начала интервала");
    }
    if (duration.isZero())
    {
        throw ValidationException("Длительность интервала не может быть нулевой");
    }
    if (startMinutes_ + duration.totalMinutes() > kMinutesPerDay)
    {
        throw ValidationException("Интервал выходит за пределы суток");
    }
}

int TimeSlot::startMinutes() const { return startMinutes_; }

int TimeSlot::endMinutes() const { return startMinutes_ + duration_.totalMinutes(); }

const Duration& TimeSlot::duration() const { return duration_; }

bool TimeSlot::overlaps(const TimeSlot& other) const
{
    return startMinutes_ < other.endMinutes() && other.startMinutes_ < endMinutes();
}

bool TimeSlot::startsBefore(const TimeSlot& other) const
{
    return startMinutes_ < other.startMinutes_;
}

bool TimeSlot::contains(int minutesFromMidnight) const
{
    return minutesFromMidnight >= startMinutes_ && minutesFromMidnight < endMinutes();
}

std::string TimeSlot::toString() const
{
    return formatMinutes(startMinutes_) + "-" + formatMinutes(endMinutes());
}

bool TimeSlot::operator==(const TimeSlot& other) const
{
    return startMinutes_ == other.startMinutes_ && duration_ == other.duration_;
}

}
