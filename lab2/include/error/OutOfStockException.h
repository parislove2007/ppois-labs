#pragma once

#include <string>
#include "error/BusinessRuleException.h"

namespace repair
{

/**
 * @brief На складе недостаточно запчастей для выполнения операции.
 */
class OutOfStockException : public BusinessRuleException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param article Артикул запчасти.
     * @param requested Запрошенное количество.
     * @param available Доступное количество.
     */
    OutOfStockException(const std::string& article, int requested, int available);

    /** @return Артикул запчасти. */
    const std::string& article() const;

    /** @return Запрошенное количество. */
    int requested() const;

    /** @return Доступное количество. */
    int available() const;

private:
    std::string article_;
    int requested_;
    int available_;
};

}
