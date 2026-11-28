#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Телефонный номер в национальном формате.
 *
 * Неизменяемый объект-значение. Хранит только цифры: все разделители,
 * скобки и пробелы отбрасываются при создании, поэтому два номера,
 * записанные по-разному, оказываются равны.
 */
class PhoneNumber
{
public:
    /**
     * @brief Создаёт номер из произвольной записи.
     * @param rawNumber Запись номера в любом формате.
     * @throws InvalidPhoneNumberException Если запись не содержит
     *         нужного количества цифр или начинается не с кода страны.
     */
    explicit PhoneNumber(const std::string& rawNumber);

    /**
     * @brief Проверяет, пригодна ли запись для создания номера.
     * @param rawNumber Проверяемая запись.
     * @return true, если из записи можно создать номер.
     */
    static bool isValid(const std::string& rawNumber);

    /** @return Номер в виде последовательности цифр. */
    const std::string& digits() const;

    /** @return Код оператора, три цифры после кода страны. */
    std::string operatorCode() const;

    /** @return Номер в виде "+375 (29) 123-45-67". */
    std::string formatted() const;

    /** @brief Сравнивает номера по цифрам. */
    bool operator==(const PhoneNumber& other) const;

private:
    std::string digits_;
};

/** @brief Записывает номер в поток в отформатированном виде. */
std::ostream& operator<<(std::ostream& out, const PhoneNumber& number);

}
