#pragma once

#include <string>
#include "error/ValidationException.h"

namespace repair
{

/**
 * @brief Серийный номер не соответствует формату.
 */
class InvalidSerialNumberException : public ValidationException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param rawSerial Отвергнутая строка серийного номера.
     */
    explicit InvalidSerialNumberException(const std::string& rawSerial);

    /** @return Отвергнутая строка серийного номера. */
    const std::string& rawSerial() const;

private:
    std::string rawSerial_;
};

}
