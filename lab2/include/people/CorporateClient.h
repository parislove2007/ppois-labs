#pragma once

#include <string>

#include "people/Client.h"

namespace repair
{

/**
 * @brief Организация, обслуживаемая по договору.
 *
 * Отличается от частного клиента фиксированной договорной скидкой,
 * которая не зависит от числа заказов и не может быть ниже
 * накопительной скидки частного клиента.
 */
class CorporateClient : public Client
{
public:
    /**
     * @brief Создаёт корпоративного клиента.
     * @param name Фамилия, имя и отчество контактного лица.
     * @param phone Контактный телефон.
     * @param email Адрес электронной почты.
     * @param address Юридический адрес.
     * @param companyName Название организации.
     * @param contractNumber Номер договора на обслуживание.
     * @param contractDiscount Договорная скидка.
     * @throws ValidationException Если название организации или номер договора пусты.
     */
    CorporateClient(const PersonName& name, const PhoneNumber& phone, const EmailAddress& email,
                    const Address& address, const std::string& companyName,
                    const std::string& contractNumber, const Percentage& contractDiscount);

    /** @return Название типа клиента. */
    std::string clientKind() const override;

    /**
     * @brief Возвращает большую из двух скидок: договорной и накопительной.
     * @return Размер скидки.
     */
    Percentage loyaltyDiscount() const override;

    /** @return Название организации. */
    const std::string& companyName() const;

    /** @return Номер договора на обслуживание. */
    const std::string& contractNumber() const;

    /** @return Договорная скидка. */
    const Percentage& contractDiscount() const;

private:
    std::string companyName_;
    std::string contractNumber_;
    Percentage contractDiscount_;
};

}
