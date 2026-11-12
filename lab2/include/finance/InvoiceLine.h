#pragma once

#include <string>

#include "value/Money.h"

namespace repair
{

/**
 * @brief Строка счёта: что именно оплачивается и на какую сумму.
 *
 * Выделена в отдельный класс, чтобы счёт можно было напечатать
 * построчно, а не одной итоговой суммой.
 */
class InvoiceLine
{
public:
    /**
     * @brief Создаёт строку счёта.
     * @param description Наименование позиции.
     * @param amount Сумма по позиции.
     * @throws ValidationException Если наименование пусто или сумма нулевая.
     */
    InvoiceLine(const std::string& description, const Money& amount);

    /** @return Наименование позиции. */
    const std::string& description() const;

    /** @return Сумма по позиции. */
    const Money& amount() const;

    /**
     * @brief Проверяет, превышает ли позиция заданный порог.
     * @param threshold Порог для сравнения.
     * @return true, если сумма позиции больше порога.
     */
    bool exceeds(const Money& threshold) const;

    /** @return Строка счёта для печати. */
    std::string describe() const;

private:
    std::string description_;
    Money amount_;
};

}
