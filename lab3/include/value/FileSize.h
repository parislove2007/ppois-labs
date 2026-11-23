#pragma once

#include <cstdint>
#include <iosfwd>

namespace fs
{

/**
 * @brief Размер данных в байтах.
 *
 * Неизменяемый объект-значение. Отрицательные значения запрещены:
 * файл не бывает размером минус два байта.
 */
class FileSize
{
public:
    /**
     * @brief Создаёт размер из числа байтов.
     * @param bytes Число байтов.
     * @throws InvalidFileSizeException Если значение отрицательно.
     */
    static FileSize fromBytes(std::int64_t bytes);

    /**
     * @brief Создаёт размер из числа килобайтов.
     * @param kilobytes Число килобайтов (1024 байта).
     * @throws InvalidFileSizeException Если значение отрицательно.
     */
    static FileSize fromKilobytes(std::int64_t kilobytes);

    /** @brief Создаёт нулевой размер. */
    FileSize();

    /** @return Число байтов. */
    std::uint64_t bytes() const;

    /** @return true, если размер равен нулю. */
    bool isZero() const;

    /**
     * @brief Проверяет, что размер помещается в лимит.
     * @param limit Допустимый предел.
     * @return true, если байтов не больше лимита.
     */
    bool fitsIn(const FileSize& limit) const;

    /** @brief Складывает размеры. */
    FileSize operator+(const FileSize& other) const;

    /**
     * @brief Вычитает размеры.
     * @throws InvalidFileSizeException Если вычитание уходит ниже нуля.
     */
    FileSize operator-(const FileSize& other) const;

    bool operator<(const FileSize& other) const;
    bool operator==(const FileSize& other) const;

private:
    explicit FileSize(std::uint64_t bytes);

    std::uint64_t bytes_;
};

/**
 * @brief Записывает размер в потоке в человекочитаемом виде (B, KiB, MiB, GiB).
 */
std::ostream& operator<<(std::ostream& out, const FileSize& size);

}
