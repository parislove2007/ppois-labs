#pragma once

#include <iosfwd>

#include "value/Money.h"

namespace repair
{

/**
 * @brief Доля от целого, выраженная в процентах.
 *
 * Неизменяемый объект-значение. Допустимый диапазон — от нуля
 * до ста процентов включительно: предметная область не знает
 * ни отрицательных долей, ни долей больше целого.
 */
class Percentage
{
public:
    /** @brief Создаёт нулевой процент. */
    Percentage();

    /**
     * @brief Создаёт процент от заданного значения.
     * @param value Величина в процентах, от нуля до ста.
     * @throws ValidationException Если значение выходит за диапазон.
     */
    explicit Percentage(int value);

    /** @return Величина в процентах. */
    int value() const;

    /** @return true, если доля нулевая. */
    bool isZero() const;

    /**
     * @brief Вычисляет долю от суммы.
     * @param amount Сумма, от которой берётся доля.
     * @return Часть суммы, соответствующая проценту.
     */
    Money applyTo(const Money& amount) const;

    /**
     * @brief Вычисляет остаток суммы после вычета доли.
     * @param amount Исходная сумма.
     * @return Сумма за вычетом доли.
     */
    Money remainderOf(const Money& amount) const;

    /**
     * @brief Складывает доли, ограничивая результат сотней процентов.
     * @param other Прибавляемая доля.
     * @return Суммарная доля, но не более ста процентов.
     */
    Percentage combinedWith(const Percentage& other) const;

    /** @brief Сравнивает доли по величине. */
    bool operator==(const Percentage& other) const;

    /** @brief Упорядочивает доли по возрастанию. */
    bool operator<(const Percentage& other) const;

private:
    int value_;
};

/** @brief Записывает долю в поток в виде "15%". */
std::ostream& operator<<(std::ostream& out, const Percentage& percentage);

}
