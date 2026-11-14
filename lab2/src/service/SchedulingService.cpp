#include "service/SchedulingService.h"

#include "error/ValidationException.h"

namespace
{
const int kMinWorkdayMinutes = 1;
const int kFirstDayOffset = 0;
}

namespace repair
{

SchedulingService::SchedulingService() {}

std::shared_ptr<Technician> SchedulingService::pickTechnician(
    const EmployeeRegistry& registry, const DeviceCategory& category) const
{
    return registry.findAvailableFor(category);
}

bool SchedulingService::canPlace(const WorkSchedule& schedule, const TimeSlot& slot) const
{
    return schedule.isFree(slot);
}

void SchedulingService::place(WorkSchedule& schedule, const TimeSlot& slot) const
{
    schedule.occupy(slot);
}

Duration SchedulingService::plannedTime(const Estimate& estimate) const
{
    Duration total;
    for (std::size_t index = 0; index < estimate.workCount(); ++index)
    {
        total = total + estimate.workAt(index).spentTime();
    }
    return total;
}

Date SchedulingService::estimatedCompletion(const Date& start, const Duration& work,
                                            int workdayMinutes) const
{
    if (workdayMinutes < kMinWorkdayMinutes)
    {
        throw ValidationException("Длительность рабочего дня должна быть положительной");
    }
    if (work.isZero())
    {
        return start;
    }
    const int fullDays = (work.totalMinutes() - 1) / workdayMinutes;
    return start.plusDays(fullDays + kFirstDayOffset);
}

}
