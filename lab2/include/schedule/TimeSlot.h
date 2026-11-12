#pragma once

#include <string>

#include "value/Duration.h"

namespace repair
{

/**
 * @brief Интервал рабочего времени внутри одного дня.
 *
 * Неизменяемый объект-значение. Время отсчитывается в минутах от
 * полуночи: так пересечение интервалов проверяется сравнением чисел,
 * без разбора часов и минут.
 */
class TimeSlot
{
public:
    /**
     * @brief Создаёт интервал.
     * @param startHour Час начала, от нуля до двадцати трёх.
     * @param startMinute Минута начала, от нуля до пятидесяти девяти.
     * @param duration Длительность интервала.
     * @throws ValidationException Если время начала недопустимо,
     *         длительность нулевая или интервал выходит за сутки.
     */
    TimeSlot(int startHour, int startMinute, const Duration& duration);

    /** @return Начало интервала в минутах от полуночи. */
    int startMinutes() const;

    /** @return Конец интервала в минутах от полуночи. */
    int endMinutes() const;

    /** @return Длительность интервала. */
    const Duration& duration() const;

    /**
     * @brief Проверяет пересечение с другим интервалом.
     * @param other Интервал для сравнения.
     * @return true, если интервалы перекрываются хотя бы на минуту.
     */
    bool overlaps(const TimeSlot& other) const;

    /**
     * @brief Проверяет, начинается ли интервал раньше другого.
     * @param other Интервал для сравнения.
     * @return true, если текущий интервал начинается раньше.
     */
    bool startsBefore(const TimeSlot& other) const;

    /**
     * @brief Проверяет попадание момента времени в интервал.
     * @param minutesFromMidnight Момент в минутах от полуночи.
     * @return true, если момент внутри интервала.
     */
    bool contains(int minutesFromMidnight) const;

    /** @return Интервал в виде "09:00-10:30". */
    std::string toString() const;

    /** @brief Сравнивает интервалы по началу и длительности. */
    bool operator==(const TimeSlot& other) const;

private:
    int startMinutes_;
    Duration duration_;
};

}
