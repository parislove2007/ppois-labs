#pragma once

#include <string>

#include "value/Money.h"
#include "value/Percentage.h"

namespace repair
{

/**
 * @brief Скидка на заказ с основанием и условием применения.
 *
 * Скидка применяется не всегда: у неё есть минимальная сумма заказа.
 * Это отражает реальное правило — скидку за объём не дают на мелкий чек.
 */
class Discount
{
public:
    /**
     * @brief Создаёт скидку.
     * @param percentage Размер скидки.
     * @param reason Основание для скидки.
     * @param minimumAmount Минимальная сумма заказа для применения.
     * @throws ValidationException Если основание не указано.
     */
    Discount(const Percentage& percentage, const std::string& reason,
             const Money& minimumAmount);

    /** @return Размер скидки. */
    const Percentage& percentage() const;

    /** @return Основание для скидки. */
    const std::string& reason() const;

    /** @return Минимальная сумма заказа для применения. */
    const Money& minimumAmount() const;

    /**
     * @brief Проверяет, применима ли скидка к сумме.
     * @param amount Сумма заказа.
     * @return true, если сумма достигла порога.
     */
    bool appliesTo(const Money& amount) const;

    /**
     * @brief Считает величину скидки для суммы.
     * @param amount Сумма заказа.
     * @return Величина скидки; ноль, если скидка неприменима.
     */
    Money amountFor(const Money& amount) const;

    /**
     * @brief Считает сумму к оплате после скидки.
     * @param amount Сумма заказа.
     * @return Сумма за вычетом скидки.
     */
    Money applyTo(const Money& amount) const;

    /**
     * @brief Сравнивает скидки по выгоде для клиента.
     * @param other Скидка для сравнения.
     * @param amount Сумма заказа.
     * @return true, если текущая скидка выгоднее на этой сумме.
     */
    bool isBetterThan(const Discount& other, const Money& amount) const;

private:
    Percentage percentage_;
    std::string reason_;
    Money minimumAmount_;
};

}
