#pragma once

#include <string>

#include "value/Permission.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Базовый пользователь файловой системы.
 *
 * Абстрактный класс: роль определяет права по умолчанию и возможность
 * превышать дисковую квоту.
 */
class User
{
public:
    /**
     * @brief Создаёт пользователя.
     * @param login Логин, уникальный в системе.
     * @param displayName Отображаемое имя.
     * @param registered Момент регистрации.
     */
    User(std::string login, std::string displayName, Timestamp registered);

    virtual ~User() = default;

    const std::string& login() const;
    const std::string& displayName() const;
    Timestamp registered() const;

    /** @return Название роли: «administrator», «regular» или «guest». */
    virtual std::string role() const = 0;

    /** @return Права, которые пользователь даёт своим файлам по умолчанию. */
    virtual Permission defaultPermission() const = 0;

    /** @return true, если пользователю разрешено превышать квоту. */
    virtual bool canBypassQuota() const;

private:
    std::string login_;
    std::string displayName_;
    Timestamp registered_;
};

}
