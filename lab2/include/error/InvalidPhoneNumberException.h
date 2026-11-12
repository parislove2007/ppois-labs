#pragma once

#include <string>
#include "error/ValidationException.h"

namespace repair
{

/**
 * @brief Телефонный номер не соответствует формату.
 */
class InvalidPhoneNumberException : public ValidationException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param rawNumber Отвергнутая строка номера.
     */
    explicit InvalidPhoneNumberException(const std::string& rawNumber);

    /** @return Отвергнутая строка номера. */
    const std::string& rawNumber() const;

private:
    std::string rawNumber_;
};

}
