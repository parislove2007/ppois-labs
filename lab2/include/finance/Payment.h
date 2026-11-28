#pragma once

#include <string>

#include "value/Date.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Способ оплаты заказа.
 */
enum class PaymentMethod
{
    Cash,      ///< Наличные в кассу.
    Card,      ///< Банковская карта.
    Transfer   ///< Безналичный перевод по счёту.
};

/**
 * @brief Переводит способ оплаты в название для документов.
 * @param method Способ оплаты.
 * @return Название способа оплаты.
 */
std::string toString(PaymentMethod method);

/**
 * @brief Платёж по счёту.
 *
 * Неизменяемая запись о свершившемся факте: платёж нельзя
 * отредактировать, ошибочный платёж отменяется встречным возвратом.
 */
class Payment
{
public:
    /**
     * @brief Создаёт запись о платеже.
     * @param amount Сумма платежа.
     * @param method Способ оплаты.
     * @param receivedOn Дата получения.
     * @param reference Номер чека или платёжного поручения.
     * @throws ValidationException Если сумма нулевая или номер не указан.
     */
    Payment(const Money& amount, PaymentMethod method, const Date& receivedOn,
            const std::string& reference);

    /** @return Сумма платежа. */
    const Money& amount() const;

    /** @return Способ оплаты. */
    PaymentMethod method() const;

    /** @return Дата получения. */
    const Date& receivedOn() const;

    /** @return Номер чека или платёжного поручения. */
    const std::string& reference() const;

    /** @return true, если оплата прошла безналичным способом. */
    bool isCashless() const;

    /**
     * @brief Проверяет, поступил ли платёж в срок.
     * @param dueDate Крайняя дата оплаты.
     * @return true, если платёж не позже крайней даты.
     */
    bool isOnTime(const Date& dueDate) const;

    /** @return Строка платежа для печати в счёте. */
    std::string describe() const;

private:
    Money amount_;
    PaymentMethod method_;
    Date receivedOn_;
    std::string reference_;
};

}
