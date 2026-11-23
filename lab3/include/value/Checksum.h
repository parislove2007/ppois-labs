#pragma once

#include <iosfwd>
#include <string>

namespace fs
{

/**
 * @brief Контрольная сумма содержимого файла.
 *
 * Неизменяемый объект-значение. Считается хэшем FNV-1a 64 бит
 * и хранится шестнадцатеричной строкой фиксированной длины.
 */
class Checksum
{
public:
    /**
     * @brief Считает контрольную сумму содержимого.
     * @param content Содержимое файла.
     * @return Контрольная сумма содержимого.
     */
    static Checksum calculate(const std::string& content);

    /** @brief Создаёт контрольную сумму пустого содержимого. */
    Checksum();

    /** @return Шестнадцатеричное представление суммы. */
    const std::string& value() const;

    /**
     * @brief Проверяет содержимое против этой суммы.
     * @param content Проверяемое содержимое.
     * @return true, если сумма содержимого совпадает.
     */
    bool verify(const std::string& content) const;

    bool operator==(const Checksum& other) const;

private:
    explicit Checksum(std::string value);

    std::string value_;
};

std::ostream& operator<<(std::ostream& out, const Checksum& checksum);

}
