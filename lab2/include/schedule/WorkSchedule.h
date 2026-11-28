#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "schedule/TimeSlot.h"
#include "value/Date.h"
#include "value/Duration.h"

namespace repair
{

/**
 * @brief График работы мастера на один день.
 *
 * Хранит занятые интервалы. Пересекающиеся интервалы добавить нельзя:
 * мастер не может одновременно вести две работы, и проверка этого
 * правила лежит здесь, а не в вызывающем коде.
 */
class WorkSchedule
{
public:
    /**
     * @brief Создаёт пустой график.
     * @param technicianName Фамилия и инициалы мастера.
     * @param date Дата, на которую составлен график.
     * @throws ValidationException Если мастер не указан.
     */
    WorkSchedule(const std::string& technicianName, const Date& date);

    /** @return Фамилия и инициалы мастера. */
    const std::string& technicianName() const;

    /** @return Дата графика. */
    const Date& date() const;

    /** @return Количество занятых интервалов. */
    std::size_t slotCount() const;

    /**
     * @brief Занимает интервал в графике.
     * @param slot Занимаемый интервал.
     * @throws BusinessRuleException Если интервал пересекается с занятым.
     */
    void occupy(const TimeSlot& slot);

    /**
     * @brief Проверяет, свободен ли интервал.
     * @param slot Проверяемый интервал.
     * @return true, если интервал не пересекается с занятыми.
     */
    bool isFree(const TimeSlot& slot) const;

    /**
     * @brief Возвращает занятый интервал по номеру.
     * @param index Номер интервала, начиная с нуля.
     * @return Занятый интервал.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const TimeSlot& slotAt(std::size_t index) const;

    /** @return Суммарное занятое время за день. */
    Duration busyTime() const;

    /**
     * @brief Считает, сколько рабочего времени осталось.
     * @param workdayMinutes Длительность рабочего дня в минутах.
     * @return Оставшееся время; ноль, если день заполнен.
     */
    Duration remainingTime(int workdayMinutes) const;

    /**
     * @brief Проверяет, поместится ли работа в оставшееся время.
     * @param work Длительность работы.
     * @param workdayMinutes Длительность рабочего дня в минутах.
     * @return true, если работа умещается.
     */
    bool hasCapacityFor(const Duration& work, int workdayMinutes) const;

private:
    std::string technicianName_;
    Date date_;
    std::vector<TimeSlot> slots_;
};

}
