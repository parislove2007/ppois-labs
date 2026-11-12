#pragma once

#include <string>
#include "error/NotFoundException.h"

namespace repair
{

/**
 * @brief Клиент с указанным телефоном не зарегистрирован.
 */
class ClientNotFoundException : public NotFoundException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param phone Телефон, по которому шёл поиск.
     */
    explicit ClientNotFoundException(const std::string& phone);

    /** @return Телефон, по которому шёл поиск. */
    const std::string& phone() const;

private:
    std::string phone_;
};

}
