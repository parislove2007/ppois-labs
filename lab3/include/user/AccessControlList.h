#pragma once

#include <map>
#include <string>

#include "value/Permission.h"

namespace fs
{

/**
 * @brief Список прав доступа: логин → права на конкретную запись.
 *
 * Нет правила для логина — нет доступа: список работает по принципу
 * «запрещено всё, что не разрешено явно».
 */
class AccessControlList
{
public:
    /**
     * @brief Выдаёт или заменяет права логину.
     * @param login Логин пользователя.
     * @param permission Выдаваемые права.
     */
    void grant(const std::string& login, Permission permission);

    /**
     * @brief Отзывает правило.
     * @throws PathNotFoundException Если правила для логина нет.
     */
    void revoke(const std::string& login);

    bool hasRuleFor(const std::string& login) const;

    /**
     * @brief Возвращает права логина.
     * @throws PathNotFoundException Если правила для логина нет.
     */
    Permission permissionFor(const std::string& login) const;

    /**
     * @brief Проверяет доступ логина к требуемым правам.
     * @return true, если правило есть и прав достаточно.
     */
    bool allows(const std::string& login, const Permission& required) const;

private:
    std::map<std::string, Permission> rules_;
};

}
