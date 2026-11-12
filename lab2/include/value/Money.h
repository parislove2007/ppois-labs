#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Денежная сумма, хранимая в копейках.
 *
 * Неизменяемый объект-значение. Хранение в целых копейках исключает
 * ошибки округления, неизбежные при использовании чисел с плавающей
 * точкой. Отрицательные суммы запрещены: все денежные величины
 * предметной области неотрицательны.
 */
class Money
{
public:
    /** @brief Создаёт нулевую сумму. */
    Money();

    /**
     * @brief Создаёт сумму из числа копеек.
     * @param kopecks Количество копеек.
     * @throws InvalidMoneyAmountException Если сумма отрицательна.
     */
    explicit Money(long long kopecks);

    /**
     * @brief Создаёт сумму из рублей и копеек.
     * @param rubles Целое число рублей.
     * @param kopecks Копейки, от нуля до девяноста девяти.
     * @return Собранная сумма.
     * @throws InvalidMoneyAmountException Если любая из частей недопустима.
     */
    static Money fromRubles(long long rubles, int kopecks);

    /** @return Полная сумма в копейках. */
    long long kopecks() const;

    /** @return Целое число рублей, содержащееся в сумме. */
    long long rubles() const;

    /** @return Остаток сумы сверх целых рублей, в копейках. */
    int kopecksPart() const;

    /** @return true, если сумма равна нулю. */
    bool isZero() const;

    /**
     * @brief Складывает две суммы.
     * @param other Прибавляемая сумма.
     * @return Сумма двух величин.
     */
    Money operator+(const Money& other) const;

    /**
     * @brief Вычитает сумму.
     * @param other Вычитаемая сумма.
     * @return Разность величин.
     * @throws InvalidMoneyAmountException Если результат отрицателен.
     */
    Money operator-(const Money& other) const;

    /**
     * @brief Повторяет сумму заданное число раз.
     * @param quantity Множитель.
     * @return Произведение суммы на множитель.
     * @throws InvalidMoneyAmountException Если множитель отрицателен.
     */
    Money operator*(int quantity) const;

    /** @brief Сравнивает суммы по величине. */
    bool operator==(const Money& other) const;

    /** @brief Упорядочивает суммы по возрастанию. */
    bool operator<(const Money& other) const;

    /** @return Сумма в виде строки вида "1234.56". */
    std::string toString() const;

private:
    long long kopecks_;
};

/** @brief Записывает сумму в поток в виде "1234.56 руб." */
std::ostream& operator<<(std::ostream& out, const Money& money);

}
