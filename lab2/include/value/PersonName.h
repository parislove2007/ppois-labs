#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Фамилия, имя и отчество человека.
 *
 * Неизменяемый объект-значение. Отчество необязательно: у части
 * клиентов его нет, поэтому пустая строка допустима, а фамилия
 * и имя обязательны.
 */
class PersonName
{
public:
    /**
     * @brief Создаёт имя без отчества.
     * @param surname Фамилия.
     * @param givenName Имя.
     * @throws ValidationException Если фамилия или имя пусты.
     */
    PersonName(const std::string& surname, const std::string& givenName);

    /**
     * @brief Создаёт полное имя.
     * @param surname Фамилия.
     * @param givenName Имя.
     * @param patronymic Отчество, может быть пустым.
     * @throws ValidationException Если фамилия или имя пусты.
     */
    PersonName(const std::string& surname, const std::string& givenName,
               const std::string& patronymic);

    /** @return Фамилия. */
    const std::string& surname() const;

    /** @return Имя. */
    const std::string& givenName() const;

    /** @return Отчество, возможно пустое. */
    const std::string& patronymic() const;

    /** @return true, если отчество указано. */
    bool hasPatronymic() const;

    /** @return Полное имя в виде "Фамилия Имя Отчество". */
    std::string fullName() const;

    /** @return Сокращённое имя в виде "Фамилия И. О." */
    std::string shortName() const;

    /** @brief Сравнивает имена по всем трём частям. */
    bool operator==(const PersonName& other) const;

private:
    std::string surname_;
    std::string givenName_;
    std::string patronymic_;
};

/** @brief Записывает имя в поток в полном виде. */
std::ostream& operator<<(std::ostream& out, const PersonName& name);

}
