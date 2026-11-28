#include "user/LockManager.h"

#include "error/FileLockedException.h"

namespace fs
{

void LockManager::lock(const Path& path, const std::string& login, Timestamp at)
{
    auto it = locks_.find(path.toString());
    if (it != locks_.end() && it->second.owner() != login)
    {
        throw FileLockedException("Locked by " + it->second.owner() + ": "
                                  + path.toString());
    }
    locks_.insert_or_assign(path.toString(), Lock(path, login, at));
}

void LockManager::unlock(const Path& path, const std::string& login)
{
    auto it = locks_.find(path.toString());
    if (it == locks_.end())
    {
        throw FileLockedException("Not locked: " + path.toString());
    }
    if (it->second.owner() != login)
    {
        throw FileLockedException("Locked by " + it->second.owner() + ": "
                                  + path.toString());
    }
    locks_.erase(it);
}

bool LockManager::isLocked(const Path& path) const
{
    return locks_.count(path.toString()) > 0;
}

std::string LockManager::ownerOf(const Path& path) const
{
    auto it = locks_.find(path.toString());
    return it == locks_.end() ? "" : it->second.owner();
}

std::size_t LockManager::releaseStale(const Timestamp& now, std::int64_t maxAgeSeconds)
{
    std::size_t released = 0;
    for (auto it = locks_.begin(); it != locks_.end();)
    {
        if (it->second.isStale(now, maxAgeSeconds))
        {
            it = locks_.erase(it);
            ++released;
        }
        else
        {
            ++it;
        }
    }
    return released;
}

}
