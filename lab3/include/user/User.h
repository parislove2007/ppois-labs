#pragma once

#include <string>

#include "value/Permission.h"
#include "value/Timestamp.h"

namespace fs
{

class UserRegistry;

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
     * @param email Электронная почта.
     * @param department Отдел, к которому приписан пользователь.
     */
    User(std::string login, std::string displayName, Timestamp registered,
         std::string email = "", std::string department = "");

    virtual ~User() = default;

    const std::string& login() const;
    const std::string& displayName() const;
    Timestamp registered() const;
    const std::string& email() const;
    const std::string& department() const;

    /** @brief Блокирует и разблокирует учётную запись. */
    void deactivate();
    void activate();
    bool isActive() const;

    /** @return Момент последнего входа или начала эпохи. */
    Timestamp lastLoginAt() const;

    /** @return Название роли: «administrator», «regular» или «guest». */
    virtual std::string role() const = 0;

    /** @return Права, которые пользователь даёт своим файлам по умолчанию. */
    virtual Permission defaultPermission() const = 0;

    /** @return true, если пользователю разрешено превышать квоту. */
    virtual bool canBypassQuota() const;

protected:
    /** @brief Отмечает успешный вход; вызывается реестром при аутентификации. */
    void markLoggedIn(Timestamp at);

private:
    friend class UserRegistry;

    std::string login_;
    std::string displayName_;
    Timestamp registered_;
    std::string email_;
    std::string department_;
    bool isActive_;
    Timestamp lastLoginAt_;
};

}
