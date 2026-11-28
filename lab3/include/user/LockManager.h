#pragma once

#include <map>
#include <string>
#include <vector>

#include "user/Lock.h"

namespace fs
{

/**
 * @brief Менеджер блокировок: эксклюзивный доступ к записям.
 *
 * Заблокированную запись может менять только владелец блокировки;
 * чужие попытки получают FileLockedException.
 */
class LockManager
{
public:
    /**
     * @brief Блокирует запись за пользователем.
     * @throws FileLockedException Если запись уже занята другим.
     */
    void lock(const Path& path, const std::string& login, Timestamp at);

    /**
     * @brief Снимает блокировку.
     * @throws FileLockedException Если записи нет или она чужая.
     */
    void unlock(const Path& path, const std::string& login);

    bool isLocked(const Path& path) const;

    /** @return Владелец блокировки или пустая строка. */
    std::string ownerOf(const Path& path) const;

    /** @brief Снимает все блокировки старше заданного возраста. */
    std::size_t releaseStale(const Timestamp& now, std::int64_t maxAgeSeconds);

private:
    std::map<std::string, Lock> locks_;
};

}
