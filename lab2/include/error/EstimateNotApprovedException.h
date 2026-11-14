#pragma once

#include <string>
#include "error/BusinessRuleException.h"

namespace repair
{

/**
 * @brief Смета по заказу не утверждена клиентом.
 */
class EstimateNotApprovedException : public BusinessRuleException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param orderNumber Номер заказа, смета которого не утверждена.
     */
    explicit EstimateNotApprovedException(int orderNumber);

    /** @return Номер заказа, смета которого не утверждена. */
    int orderNumber() const;

private:
    int orderNumber_;
};

}
