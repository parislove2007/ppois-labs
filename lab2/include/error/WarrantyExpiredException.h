#pragma once

#include <string>
#include "error/BusinessRuleException.h"

namespace repair
{

/**
 * @brief Срок гарантии по заказу истёк.
 */
class WarrantyExpiredException : public BusinessRuleException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param orderNumber Номер заказа, по которому проверялась гарантия.
     * @param expiryDate Дата окончания гарантии.
     */
    WarrantyExpiredException(int orderNumber, const std::string& expiryDate);

    /** @return Номер заказа, по которому проверялась гарантия. */
    int orderNumber() const;

    /** @return Дата окончания гарантии. */
    const std::string& expiryDate() const;

private:
    int orderNumber_;
    std::string expiryDate_;
};

}
