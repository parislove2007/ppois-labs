#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Длительность работы, измеряемая в минутах.
 *
 * Неизменяемый объект-значение. Отрицательная длительность не имеет
 * смысла в предметной области и запрещена конструктором.
 */
class Duration
{
public:
    /** @brief Создаёт нулевую длительность. */
    Duration();

    /**
     * @brief Создаёт длительность из числа минут.
     * @param minutes Количество минут.
     * @throws ValidationException Если число минут отрицательно.
     */
    explicit Duration(int minutes);

    /**
     * @brief Создаёт длительность из часов и минут.
     * @param hours Количество часов.
     * @param minutes Количество минут сверх часов.
     * @return Собранная длительность.
     * @throws ValidationException Если любая из частей отрицательна.
     */
    static Duration fromHours(int hours, int minutes);

    /** @return Полная длительность в минутах. */
    int totalMinutes() const;

    /** @return Целое число часов в длительности. */
    int hours() const;

    /** @return Остаток длительности сверх целых часов, в минутах. */
    int minutesPart() const;

    /** @return true, если длительность нулевая. */
    bool isZero() const;

    /**
     * @brief Складывает длительности.
     * @param other Прибавляемая длительность.
     * @return Суммарная длительность.
     */
    Duration operator+(const Duration& other) const;

    /**
     * @brief Проверяет, превышает ли длительность заданную.
     * @param other Длительность для сравнения.
     * @return true, если текущая длительность больше.
     */
    bool isLongerThan(const Duration& other) const;

    /** @brief Сравнивает длительности по величине. */
    bool operator==(const Duration& other) const;

    /** @return Длительность в виде строки вида "2 ч 30 мин". */
    std::string toString() const;

private:
    int minutes_;
};

/** @brief Записывает длительность в поток. */
std::ostream& operator<<(std::ostream& out, const Duration& duration);

}
