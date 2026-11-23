#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "user/User.h"

namespace fs
{

/**
 * @brief Каталог пользователей: регистрация и аутентификация.
 *
 * Пароль хранится хэшем контрольной суммы; неверный ввод увеличивает
 * счётчик неудач, после трёх подряд учётная запись блокируется.
 */
class UserRegistry
{
public:
    explicit UserRegistry(std::size_t maxFailedAttempts);

    /**
     * @brief Регистрирует пользователя с паролем.
     * @throws EntryAlreadyExistsException Если логин занят.
     */
    void registerUser(std::unique_ptr<User> user, const std::string& password);

    /** @return Пользователь по логину или nullptr. */
    User* findByLogin(const std::string& login) const;

    /**
     * @brief Проверяет корректность пароля.
     * @throws PermissionDeniedException Если пароль неверен или вход заблокирован.
     */
    User& authenticate(const std::string& login, const std::string& password);

    /** @return Логины всех зарегистрированных пользователей. */
    std::vector<std::string> logins() const;

private:
    struct Credentials
    {
        std::string passwordHash_;
        std::size_t failedAttempts_ = 0;
        bool blocked_ = false;
    };

    std::map<std::string, std::unique_ptr<User>> users_;
    std::map<std::string, Credentials> credentials_;
    std::size_t maxFailedAttempts_;
};

}
