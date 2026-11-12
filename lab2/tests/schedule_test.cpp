#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"
#include "people/Client.h"
#include "schedule/Appointment.h"
#include "schedule/TimeSlot.h"
#include "schedule/WorkSchedule.h"

using namespace repair;

namespace
{
const int kWorkdayMinutes = 480;

TimeSlot morning() { return TimeSlot(9, 0, Duration::fromHours(1, 30)); }

TimeSlot afternoon() { return TimeSlot(14, 0, Duration::fromHours(2, 0)); }

std::shared_ptr<Client> sampleClient()
{
    return std::make_shared<Client>(PersonName("Иванов", "Пётр"), PhoneNumber("375291234567"),
                                    EmailAddress("client@example.com"),
                                    Address("Минск", "Лесная", "7"));
}

Appointment sampleAppointment()
{
    return Appointment(sampleClient(), Date(2026, 1, 20), morning(), "Приём в ремонт");
}
}

TEST(TimeSlotTest, ComputesBoundariesAndText)
{
    const TimeSlot slot = morning();

    EXPECT_EQ(slot.startMinutes(), 540);
    EXPECT_EQ(slot.endMinutes(), 630);
    EXPECT_EQ(slot.duration().totalMinutes(), 90);
    EXPECT_EQ(slot.toString(), "09:00-10:30");
    EXPECT_TRUE(slot == TimeSlot(9, 0, Duration(90)));
}

TEST(TimeSlotTest, DetectsOverlapAndOrder)
{
    const TimeSlot first = morning();
    const TimeSlot overlapping(10, 0, Duration(60));
    const TimeSlot adjacent(10, 30, Duration(60));

    EXPECT_TRUE(first.overlaps(overlapping));
    EXPECT_FALSE(first.overlaps(adjacent));
    EXPECT_TRUE(first.startsBefore(afternoon()));
    EXPECT_FALSE(afternoon().startsBefore(first));
}

TEST(TimeSlotTest, ChecksMomentInsideSlot)
{
    const TimeSlot slot = morning();

    EXPECT_TRUE(slot.contains(540));
    EXPECT_TRUE(slot.contains(629));
    EXPECT_FALSE(slot.contains(630));
    EXPECT_FALSE(slot.contains(539));
}

TEST(TimeSlotTest, RejectsImpossibleSlots)
{
    EXPECT_THROW(TimeSlot(-1, 0, Duration(30)), ValidationException);
    EXPECT_THROW(TimeSlot(24, 0, Duration(30)), ValidationException);
    EXPECT_THROW(TimeSlot(9, 60, Duration(30)), ValidationException);
    EXPECT_THROW(TimeSlot(9, 0, Duration(0)), ValidationException);
    EXPECT_THROW(TimeSlot(23, 30, Duration(60)), ValidationException);
}

TEST(WorkScheduleTest, OccupiesNonOverlappingSlots)
{
    WorkSchedule schedule("Сидоров И. ", Date(2026, 1, 20));
    schedule.occupy(morning());
    schedule.occupy(afternoon());

    EXPECT_EQ(schedule.slotCount(), 2u);
    EXPECT_EQ(schedule.busyTime().totalMinutes(), 210);
    EXPECT_EQ(schedule.technicianName(), "Сидоров И. ");
    EXPECT_TRUE(schedule.date() == Date(2026, 1, 20));
    EXPECT_TRUE(schedule.slotAt(0) == morning());
}

TEST(WorkScheduleTest, RefusesOverlappingSlot)
{
    WorkSchedule schedule("Сидоров И. ", Date(2026, 1, 20));
    schedule.occupy(morning());

    EXPECT_FALSE(schedule.isFree(TimeSlot(10, 0, Duration(60))));
    EXPECT_THROW(schedule.occupy(TimeSlot(10, 0, Duration(60))), BusinessRuleException);
    EXPECT_TRUE(schedule.isFree(afternoon()));
}

TEST(WorkScheduleTest, ComputesRemainingCapacity)
{
    WorkSchedule schedule("Сидоров И. ", Date(2026, 1, 20));
    schedule.occupy(morning());

    EXPECT_EQ(schedule.remainingTime(kWorkdayMinutes).totalMinutes(), 390);
    EXPECT_TRUE(schedule.hasCapacityFor(Duration(390), kWorkdayMinutes));
    EXPECT_FALSE(schedule.hasCapacityFor(Duration(391), kWorkdayMinutes));
    EXPECT_EQ(schedule.remainingTime(30).totalMinutes(), 0);
}

TEST(WorkScheduleTest, ValidatesOwnerAndIndex)
{
    const WorkSchedule schedule("Сидоров И. ", Date(2026, 1, 20));

    EXPECT_THROW(WorkSchedule("", Date(2026, 1, 20)), ValidationException);
    EXPECT_THROW(schedule.slotAt(0), NotFoundException);
}

TEST(AppointmentTest, StoresClientDateAndSlot)
{
    const Appointment appointment = sampleAppointment();

    EXPECT_FALSE(appointment.isCancelled());
    EXPECT_TRUE(appointment.date() == Date(2026, 1, 20));
    EXPECT_TRUE(appointment.slot() == morning());
    EXPECT_EQ(appointment.purpose(), "Приём в ремонт");
    EXPECT_EQ(appointment.client().clientKind(), "Частный клиент");
    EXPECT_EQ(appointment.describe(),
              "20.01.2026 09:00-10:30, Иванов П. — Приём в ремонт");
}

TEST(AppointmentTest, ReschedulesWhileActive)
{
    Appointment appointment = sampleAppointment();
    appointment.reschedule(Date(2026, 1, 21), afternoon());

    EXPECT_TRUE(appointment.date() == Date(2026, 1, 21));
    EXPECT_TRUE(appointment.slot() == afternoon());
}

TEST(AppointmentTest, CancelsOnlyOnceAndBlocksReschedule)
{
    Appointment appointment = sampleAppointment();
    appointment.cancel();

    EXPECT_TRUE(appointment.isCancelled());
    EXPECT_THROW(appointment.cancel(), BusinessRuleException);
    EXPECT_THROW(appointment.reschedule(Date(2026, 1, 21), afternoon()),
                 BusinessRuleException);
    EXPECT_NE(appointment.describe().find("отменена"), std::string::npos);
}

TEST(AppointmentTest, DetectsConflictsOnlyBetweenActiveRecords)
{
    Appointment first = sampleAppointment();
    const Appointment overlapping(sampleClient(), Date(2026, 1, 20),
                                  TimeSlot(10, 0, Duration(60)), "Выдача");
    const Appointment otherDay(sampleClient(), Date(2026, 1, 21), morning(), "Выдача");

    EXPECT_TRUE(first.conflictsWith(overlapping));
    EXPECT_FALSE(first.conflictsWith(otherDay));

    first.cancel();
    EXPECT_FALSE(first.conflictsWith(overlapping));
}

TEST(AppointmentTest, ValidatesClientAndPurpose)
{
    EXPECT_THROW(Appointment(nullptr, Date(2026, 1, 20), morning(), "Приём"),
                 ValidationException);
    EXPECT_THROW(Appointment(sampleClient(), Date(2026, 1, 20), morning(), ""),
                 ValidationException);
}
