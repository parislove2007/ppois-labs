#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "finance/Discount.h"
#include "finance/InvoiceLine.h"
#include "finance/Payment.h"
#include "value/Date.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Счёт на оплату выполненного ремонта.
 *
 * Состав счёта и скидка фиксируются до первого платежа: менять сумму
 * после того, как клиент начал платить, нельзя. Переплата не
 * принимается — остаток к оплате никогда не уходит в минус.
 */
class Invoice
{
public:
    /**
     * @brief Создаёт пустой счёт.
     * @param number Номер счёта.
     * @param orderNumber Номер заказа, по которому выставлен счёт.
     * @param issuedOn Дата выставления.
     * @throws ValidationException Если номер счёта или заказа не положителен.
     */
    Invoice(int number, int orderNumber, const Date& issuedOn);

    /** @return Номер счёта. */
    int number() const;

    /** @return Номер заказа. */
    int orderNumber() const;

    /** @return Дата выставления. */
    const Date& issuedOn() const;

    /** @return Количество позиций. */
    std::size_t lineCount() const;

    /** @return Количество зарегистрированных платежей. */
    std::size_t paymentCount() const;

    /**
     * @brief Добавляет позицию в счёт.
     * @param line Позиция счёта.
     * @throws BusinessRuleException Если по счёту уже есть платежи.
     */
    void addLine(const InvoiceLine& line);

    /**
     * @brief Применяет скидку к счёту.
     * @param discount Применяемая скидка.
     * @throws BusinessRuleException Если по счёту уже есть платежи.
     */
    void applyDiscount(const Discount& discount);

    /** @return Сумма всех позиций до скидки. */
    Money subtotal() const;

    /** @return Величина применённой скидки. */
    Money discountAmount() const;

    /** @return Сумма к оплате с учётом скидки. */
    Money total() const;

    /** @return Фактически оплаченная сумма. */
    Money paidAmount() const;

    /** @return Остаток к оплате. */
    Money balanceDue() const;

    /** @return true, если счёт оплачен полностью. */
    bool isPaid() const;

    /**
     * @brief Регистрирует платёж по счёту.
     * @param payment Поступивший платёж.
     * @throws BusinessRuleException Если счёт уже оплачен или платёж
     *         превышает остаток.
     */
    void registerPayment(const Payment& payment);

    /**
     * @brief Возвращает позицию счёта по номеру.
     * @param index Номер позиции, начиная с нуля.
     * @return Позиция счёта.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const InvoiceLine& lineAt(std::size_t index) const;

    /**
     * @brief Возвращает платёж по номеру.
     * @param index Номер платежа, начиная с нуля.
     * @return Платёж.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const Payment& paymentAt(std::size_t index) const;

private:
    int number_;
    int orderNumber_;
    Date issuedOn_;
    std::vector<InvoiceLine> lines_;
    std::vector<Payment> payments_;
    std::optional<Discount> discount_;
};

}
