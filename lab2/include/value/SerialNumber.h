#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Серийный номер экземпляра техники.
 *
 * Неизменяемый объект-значение. Допускаются заглавные латинские буквы,
 * цифры и дефис; строчные буквы приводятся к заглавным, чтобы один и
 * тот же номер, записанный в разном регистре, считался одним.
 */
class SerialNumber
{
public:
    /**
     * @brief Создаёт серийный номер из строки.
     * @param rawSerial Запись номера.
     * @throws InvalidSerialNumberException Если запись содержит
     *         недопустимые символы или имеет неподходящую длину.
     */
    explicit SerialNumber(const std::string& rawSerial);

    /**
     * @brief Проверяет, пригодна ли строка для создания номера.
     * @param rawSerial Проверяемая строка.
     * @return true, если из строки можно создать номер.
     */
    static bool isValid(const std::string& rawSerial);

    /** @return Номер, приведённый к заглавным буквам. */
    const std::string& value() const;

    /**
     * @brief Выделяет префикс партии.
     * @param length Количество символов в префиксе.
     * @return Начальные символы номера.
     * @throws ValidationException Если длина превышает длину номера.
     */
    std::string batchPrefix(std::size_t length) const;

    /** @brief Сравнивает номера посимвольно. */
    bool operator==(const SerialNumber& other) const;

    /** @brief Упорядочивает номера лексикографически. */
    bool operator<(const SerialNumber& other) const;

private:
    std::string value_;
};

/** @brief Записывает серийный номер в поток. */
std::ostream& operator<<(std::ostream& out, const SerialNumber& serial);

}
