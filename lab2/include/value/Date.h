#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Календарная дата без времени суток.
 *
 * Неизменяемый объект-значение. Арифметика и проверка корректности
 * опираются на календарь из заголовка <chrono>, появившийся в C++20:
 * это избавляет от ручной обработки високосных годов и разной длины
 * месяцев.
 */
class Date
{
public:
    /**
     * @brief Создаёт дату.
     * @param year Год.
     * @param month Месяц, от одного до двенадцати.
     * @param day День месяца.
     * @throws ValidationException Если такой даты не существует.
     */
    Date(int year, int month, int day);

    /**
     * @brief Проверяет существование даты в календаре.
     * @param year Год.
     * @param month Месяц.
     * @param day День месяца.
     * @return true, если такая дата существует.
     */
    static bool isValid(int year, int month, int day);

    /** @return Год. */
    int year() const;

    /** @return Месяц. */
    int month() const;

    /** @return День месяца. */
    int day() const;

    /**
     * @brief Считает количество дней до другой даты.
     * @param other Конечная дата.
     * @return Число дней; отрицательное, если другая дата раньше.
     */
    int daysUntil(const Date& other) const;

    /**
     * @brief Сдвигает дату на заданное число дней.
     * @param days Число дней, может быть отрицательным.
     * @return Новая дата.
     */
    Date plusDays(int days) const;

    /**
     * @brief Сдвигает дату на заданное число месяцев.
     *
     * Если в целевом месяце нет такого дня, берётся последний день месяца.
     * @param months Число месяцев, может быть отрицательным.
     * @return Новая дата.
     */
    Date plusMonths(int months) const;

    /**
     * @brief Проверяет, предшествует ли дата другой.
     * @param other Дата для сравнения.
     * @return true, если текущая дата раньше.
     */
    bool isBefore(const Date& other) const;

    /**
     * @brief Проверяет, следует ли дата за другой.
     * @param other Дата для сравнения.
     * @return true, если текущая дата позже.
     */
    bool isAfter(const Date& other) const;

    /** @return Дата в виде строки "ДД.ММ.ГГГГ". */
    std::string toString() const;

    /** @brief Сравнивает даты по всем частям. */
    bool operator==(const Date& other) const;

    /** @brief Упорядочивает даты по возрастанию. */
    bool operator<(const Date& other) const;

private:
    int year_;
    int month_;
    int day_;
};

/** @brief Записывает дату в поток в виде "ДД.ММ.ГГГГ". */
std::ostream& operator<<(std::ostream& out, const Date& date);

}
