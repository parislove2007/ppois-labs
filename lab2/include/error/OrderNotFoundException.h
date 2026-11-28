#pragma once

#include <string>
#include "error/NotFoundException.h"

namespace repair
{

/**
 * @brief Заказ с указанным номером отсутствует в журнале.
 */
class OrderNotFoundException : public NotFoundException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param orderNumber Номер заказа, по которому шёл поиск.
     */
    explicit OrderNotFoundException(int orderNumber);

    /** @return Номер заказа, по которому шёл поиск. */
    int orderNumber() const;

private:
    int orderNumber_;
};

}
