#include "schedule/WorkSchedule.h"

#include <algorithm>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace
{
const int kNoMinutes = 0;
}

namespace repair
{

WorkSchedule::WorkSchedule(const std::string& technicianName, const Date& date)
    : technicianName_(technicianName), date_(date)
{
    if (technicianName.empty())
    {
        throw ValidationException("Мастер, которому принадлежит график, не указан");
    }
}

const std::string& WorkSchedule::technicianName() const { return technicianName_; }

const Date& WorkSchedule::date() const { return date_; }

std::size_t WorkSchedule::slotCount() const { return slots_.size(); }

bool WorkSchedule::isFree(const TimeSlot& slot) const
{
    return std::none_of(slots_.begin(), slots_.end(),
                        [&slot](const TimeSlot& busy) { return busy.overlaps(slot); });
}

void WorkSchedule::occupy(const TimeSlot& slot)
{
    if (!isFree(slot))
    {
        throw BusinessRuleException("Интервал " + slot.toString()
                                    + " пересекается с уже занятым");
    }
    slots_.push_back(slot);
}

const TimeSlot& WorkSchedule::slotAt(std::size_t index) const
{
    if (index >= slots_.size())
    {
        throw NotFoundException("Интервал с таким номером отсутствует в графике");
    }
    return slots_[index];
}

Duration WorkSchedule::busyTime() const
{
    int minutes = kNoMinutes;
    for (const TimeSlot& slot : slots_)
    {
        minutes += slot.duration().totalMinutes();
    }
    return Duration(minutes);
}

Duration WorkSchedule::remainingTime(int workdayMinutes) const
{
    const int free = workdayMinutes - busyTime().totalMinutes();
    return Duration(free < kNoMinutes ? kNoMinutes : free);
}

bool WorkSchedule::hasCapacityFor(const Duration& work, int workdayMinutes) const
{
    return !work.isLongerThan(remainingTime(workdayMinutes));
}

}
