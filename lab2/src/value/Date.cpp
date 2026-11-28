#include "value/Date.h"

#include <chrono>
#include <ostream>

#include "error/ValidationException.h"

namespace
{
const std::size_t kDayDigits = 2;
const std::size_t kMonthDigits = 2;

std::chrono::year_month_day toCalendar(int year, int month, int day)
{
    return std::chrono::year_month_day{
        std::chrono::year{year},
        std::chrono::month{static_cast<unsigned>(month)},
        std::chrono::day{static_cast<unsigned>(day)}};
}

std::chrono::sys_days toDayCount(int year, int month, int day)
{
    return std::chrono::sys_days{toCalendar(year, month, day)};
}

std::string padded(int number, std::size_t width)
{
    std::string text = std::to_string(number);
    while (text.size() < width)
    {
        text = "0" + text;
    }
    return text;
}
}

namespace repair
{

Date::Date(int year, int month, int day) : year_(year), month_(month), day_(day)
{
    if (!isValid(year, month, day))
    {
        throw ValidationException("Такой календарной даты не существует: "
                                  + std::to_string(day) + "." + std::to_string(month)
                                  + "." + std::to_string(year));
    }
}

bool Date::isValid(int year, int month, int day)
{
    const bool monthInRange = month >= 1 && month <= 12;
    const bool dayIsPositive = day >= 1;
    if (!monthInRange || !dayIsPositive)
    {
        return false;
    }
    return toCalendar(year, month, day).ok();
}

int Date::year() const { return year_; }

int Date::month() const { return month_; }

int Date::day() const { return day_; }

int Date::daysUntil(const Date& other) const
{
    const auto from = toDayCount(year_, month_, day_);
    const auto to = toDayCount(other.year_, other.month_, other.day_);
    return static_cast<int>((to - from).count());
}

Date Date::plusDays(int days) const
{
    const auto shifted = toDayCount(year_, month_, day_) + std::chrono::days{days};
    const std::chrono::year_month_day result{shifted};
    return Date(static_cast<int>(result.year()),
                static_cast<int>(static_cast<unsigned>(result.month())),
                static_cast<int>(static_cast<unsigned>(result.day())));
}

Date Date::plusMonths(int months) const
{
    auto shifted = toCalendar(year_, month_, day_) + std::chrono::months{months};
    if (!shifted.ok())
    {
        shifted = shifted.year() / shifted.month() / std::chrono::last;
    }
    return Date(static_cast<int>(shifted.year()),
                static_cast<int>(static_cast<unsigned>(shifted.month())),
                static_cast<int>(static_cast<unsigned>(shifted.day())));
}

bool Date::isBefore(const Date& other) const { return daysUntil(other) > 0; }

bool Date::isAfter(const Date& other) const { return daysUntil(other) < 0; }

std::string Date::toString() const
{
    return padded(day_, kDayDigits) + "." + padded(month_, kMonthDigits)
           + "." + std::to_string(year_);
}

bool Date::operator==(const Date& other) const
{
    return year_ == other.year_ && month_ == other.month_ && day_ == other.day_;
}

bool Date::operator<(const Date& other) const { return isBefore(other); }

std::ostream& operator<<(std::ostream& out, const Date& date)
{
    out << date.toString();
    return out;
}

}
