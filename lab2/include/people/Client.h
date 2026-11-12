#pragma once

#include <string>

#include "value/Address.h"
#include "value/EmailAddress.h"
#include "value/PersonName.h"
#include "value/PhoneNumber.h"
#include "value/Percentage.h"

namespace repair
{

/**
 * @brief Частный клиент сервисного центра.
 *
 * Телефон служит ключом: по нему клиент ищется в реестре. Размер
 * накопительной скидки зависит от числа завершённых заказов, поэтому
 * вычисляется, а не хранится.
 */
class Client
{
public:
    /**
     * @brief Создаёт клиента.
     * @param name Фамилия, имя и отчество.
     * @param phone Контактный телефон.
     * @param email Адрес электронной почты.
     * @param address Почтовый адрес.
     */
    Client(const PersonName& name, const PhoneNumber& phone, const EmailAddress& email,
           const Address& address);

    /** @brief Виртуальный деструктор: класс предназначен для наследования. */
    virtual ~Client();

    /** @return Фамилия, имя и отчество. */
    const PersonName& name() const;

    /** @return Контактный телефон. */
    const PhoneNumber& phone() const;

    /** @return Адрес электронной почты. */
    const EmailAddress& email() const;

    /** @return Почтовый адрес. */
    const Address& address() const;

    /** @return Количество завершённых заказов. */
    int completedOrders() const;

    /** @return Название типа клиента. */
    virtual std::string clientKind() const;

    /**
     * @brief Считает накопительную скидку по числу завершённых заказов.
     * @return Размер скидки.
     */
    virtual Percentage loyaltyDiscount() const;

    /** @brief Отмечает ещё один завершённый заказ. */
    void registerCompletedOrder();

    /**
     * @brief Проверяет, является ли клиент постоянным.
     * @param requiredOrders Число заказов, после которого клиент считается постоянным.
     * @return true, если порог достигнут.
     */
    bool isRegular(int requiredOrders) const;

    /** @return Строка с именем и телефоном для списков и уведомлений. */
    std::string contactLine() const;

private:
    PersonName name_;
    PhoneNumber phone_;
    EmailAddress email_;
    Address address_;
    int completedOrders_;
};

}
