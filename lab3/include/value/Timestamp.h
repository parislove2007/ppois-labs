#pragma once

#include <cstdint>
#include <iosfwd>

namespace fs
{

/**
 * @brief Момент времени с точностью до секунды.
 *
 * Неизменяемый объект-значение; секунды отсчитываются от эпохи Unix.
 */
class Timestamp
{
public:
    /**
     * @brief Создаёт момент из числа секунд.
     * @param seconds Секунды от эпохи Unix.
     */
    static Timestamp of(std::int64_t seconds);

    /** @brief Создаёт момент начала эпохи. */
    Timestamp();

    /** @return Число секунд от эпохи Unix. */
    std::int64_t seconds() const;

    /**
     * @brief Сдвигает момент на заданное число секунд.
     * @param delta Секунды, могут быть отрицательными.
     */
    Timestamp plusSeconds(std::int64_t delta) const;

    /**
     * @brief Считает разницу с другим моментом.
     * @param other Другой момент.
     * @return Секунды между моментами; положительные, если other раньше.
     */
    std::int64_t diffSeconds(const Timestamp& other) const;

    bool isBefore(const Timestamp& other) const;
    bool isAfter(const Timestamp& other) const;
    bool operator==(const Timestamp& other) const;

private:
    explicit Timestamp(std::int64_t seconds);

    std::int64_t seconds_;
};

std::ostream& operator<<(std::ostream& out, const Timestamp& timestamp);

}
