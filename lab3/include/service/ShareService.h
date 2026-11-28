#pragma once

#include <map>
#include <string>

#include "service/ShareLink.h"
#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Публикация записей по токенам с ограниченным сроком жизни.
 */
class ShareService
{
public:
    ShareService();

    /**
     * @brief Публикует запись и возвращает токен ссылки.
     * @param path Публикуемый путь.
     * @param owner Логин публикующего.
     * @param now Момент публикации.
     * @param ttlSeconds Время жизни ссылки в секундах.
     */
    std::string createShare(const Path& path, const std::string& owner, Timestamp now,
                            std::int64_t ttlSeconds);

    /**
     * @brief Находит активную ссылку по токену.
     * @throws PathNotFoundException Если токен неизвестен или отозван.
     * @throws PermissionDeniedException Если срок ссылки истёк.
     */
    const ShareLink& resolve(const std::string& token, Timestamp now) const;

    /**
     * @brief Отзывает ссылку.
     * @throws PathNotFoundException Если токен неизвестен.
     */
    void revoke(const std::string& token);

    /** @return Число активных ссылок. */
    std::size_t activeCount() const;

private:
    std::map<std::string, ShareLink> links_;
    long nextId_;
};

}
