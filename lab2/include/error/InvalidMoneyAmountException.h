#pragma once

#include <string>
#include "error/ValidationException.h"

namespace repair
{

/**
 * @brief Недопустимая денежная сумма.
 */
class InvalidMoneyAmountException : public ValidationException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param kopecks Отвергнутая сумма в копейках.
     */
    explicit InvalidMoneyAmountException(long long kopecks);

    /** @return Отвергнутая сумма в копейках. */
    long long kopecks() const;

private:
    long long kopecks_;
};

}
