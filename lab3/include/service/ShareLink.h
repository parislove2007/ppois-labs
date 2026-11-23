#pragma once

#include <string>

#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Публичная ссылка на запись: токен, срок жизни и владелец.
 */
class ShareLink
{
public:
    ShareLink(std::string token, Path path, std::string owner, Timestamp createdAt,
              Timestamp expiresAt);

    const std::string& token() const;
    const Path& path() const;
    const std::string& owner() const;
    Timestamp createdAt() const;
    Timestamp expiresAt() const;

    /** @return true, если срок ссылки истёк к указанному моменту. */
    bool isExpired(const Timestamp& now) const;

    /** @brief Регистрирует обращение по ссылке; допустим и для константных ссылок. */
    void registerHit() const;
    int hits() const;

    /** @brief Отзывает ссылку досрочно. */
    void revoke();
    bool isRevoked() const;

private:
    std::string token_;
    Path path_;
    std::string owner_;
    Timestamp createdAt_;
    Timestamp expiresAt_;
    mutable int hits_;
    bool revoked_;
};

}
