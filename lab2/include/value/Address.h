#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Почтовый адрес клиента или организации.
 *
 * Неизменяемый объект-значение. Номер квартиры необязателен: для
 * частного дома он отсутствует, и это нормальное состояние, а не ошибка.
 */
class Address
{
public:
    /**
     * @brief Создаёт адрес частного дома.
     * @param city Населённый пункт.
     * @param street Улица.
     * @param house Номер дома.
     * @throws ValidationException Если обязательная часть пуста.
     */
    Address(const std::string& city, const std::string& street, const std::string& house);

    /**
     * @brief Создаёт адрес квартиры.
     * @param city Населённый пункт.
     * @param street Улица.
     * @param house Номер дома.
     * @param apartment Номер квартиры, может быть пустым.
     * @throws ValidationException Если обязательная часть пуста.
     */
    Address(const std::string& city, const std::string& street, const std::string& house,
            const std::string& apartment);

    /** @return Населённый пункт. */
    const std::string& city() const;

    /** @return Улица. */
    const std::string& street() const;

    /** @return Номер дома. */
    const std::string& house() const;

    /** @return Номер квартиры, возможно пустой. */
    const std::string& apartment() const;

    /** @return true, если квартира не указана. */
    bool isPrivateHouse() const;

    /**
     * @brief Проверяет совпадение населённого пункта.
     * @param other Адрес для сравнения.
     * @return true, если оба адреса в одном населённом пункте.
     */
    bool isSameCity(const Address& other) const;

    /** @return Адрес, записанный одной строкой. */
    std::string oneLine() const;

    /** @brief Сравнивает адреса по всем частям. */
    bool operator==(const Address& other) const;

private:
    std::string city_;
    std::string street_;
    std::string house_;
    std::string apartment_;
};

/** @brief Записывает адрес в поток одной строкой. */
std::ostream& operator<<(std::ostream& out, const Address& address);

}
