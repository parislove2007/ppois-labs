#pragma once

#include <iosfwd>

/**
 * @brief Точка на плоскости с целочисленными координатами.
 *
 * Неизменяемая: после создания координаты поменять нельзя,
 * можно только создать новую точку.
 */
class Point
{
public:
    /** @brief Создаёт точку в начале координат. */
    Point();

    /**
     * @brief Создаёт точку с заданными координатами.
     * @param x Координата по горизонтальной оси.
     * @param y Координата по вертикальной оси.
     */
    Point(int x, int y);

    /** @return Координата по горизонтальной оси. */
    int x() const;

    /** @return Координата по вертикальной оси. */
    int y() const;

    /**
     * @brief Сравнивает точки на равенство.
     * @param other Точка для сравнения.
     * @return true, если обе координаты совпадают.
     */
    bool operator==(const Point& other) const;

private:
    int x_;
    int y_;
};

/**
 * @brief Записывает точку в поток в формате "x y".
 * @param out Поток вывода.
 * @param point Записываемая точка.
 * @return Ссылка на поток для построения цепочек.
 */
std::ostream& operator<<(std::ostream& out, const Point& point);

/**
 * @brief Читает точку из потока в формате "x y".
 *
 * При некорректном вводе поток переводится в состояние ошибки,
 * а точка остаётся неизменной.
 * @param in Поток ввода.
 * @param point Точка, которая будет заполнена.
 * @return Ссылка на поток.
 */
std::istream& operator>>(std::istream& in, Point& point);