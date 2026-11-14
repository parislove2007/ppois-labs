#pragma once

#include <string>
#include "error/BusinessRuleException.h"

namespace repair
{

/**
 * @brief Недопустимый переход между статусами заказа.
 */
class InvalidStatusTransitionException : public BusinessRuleException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param fromStatus Статус, из которого выполнялся переход.
     * @param toStatus Статус, в который выполнялся переход.
     */
    InvalidStatusTransitionException(const std::string& fromStatus, const std::string& toStatus);

    /** @return Статус, из которого выполнялся переход. */
    const std::string& fromStatus() const;

    /** @return Статус, в который выполнялся переход. */
    const std::string& toStatus() const;

private:
    std::string fromStatus_;
    std::string toStatus_;
};

}
