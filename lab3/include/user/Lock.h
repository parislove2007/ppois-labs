#pragma once

#include <string>

#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Блокировка файла: кто и когда занял запись.
 */
class Lock
{
public:
    Lock(Path path, std::string owner, Timestamp at);

    const Path& path() const;
    const std::string& owner() const;
    Timestamp lockedAt() const;

    /**
     * @brief Проверяет, что блокировка старше заданного возраста.
     * @param now Текущий момент.
     * @param maxAgeSeconds Допустимый возраст в секундах.
     */
    bool isStale(const Timestamp& now, std::int64_t maxAgeSeconds) const;

private:
    Path path_;
    std::string owner_;
    Timestamp lockedAt_;
};

}
