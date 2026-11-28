#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Адрес электронной почты.
 *
 * Неизменяемый объект-значение. Проверка намеренно простая: адрес
 * должен содержать ровно один символ "@", непустые части до и после
 * него, и точку в доменной части.
 */
class EmailAddress
{
public:
    /**
     * @brief Создаёт адрес из строки.
     * @param value Запись адреса.
     * @throws ValidationException Если запись не похожа на адрес почты.
     */
    explicit EmailAddress(const std::string& value);

    /**
     * @brief Проверяет, пригодна ли строка для создания адреса.
     * @param value Проверяемая строка.
     * @return true, если из строки можно создать адрес.
     */
    static bool isValid(const std::string& value);

    /** @return Полная запись адреса. */
    const std::string& value() const;

    /** @return Часть адреса до символа "@". */
    std::string localPart() const;

    /** @return Доменная часть адреса после символа "@". */
    std::string domain() const;

    /**
     * @brief Проверяет принадлежность адреса домену.
     * @param expectedDomain Проверяемый домен.
     * @return true, если доменная часть совпадает.
     */
    bool belongsToDomain(const std::string& expectedDomain) const;

    /** @brief Сравнивает адреса посимвольно. */
    bool operator==(const EmailAddress& other) const;

private:
    std::string value_;
};

/** @brief Записывает адрес в поток. */
std::ostream& operator<<(std::ostream& out, const EmailAddress& address);

}
