#include "user/Lock.h"

namespace fs
{

Lock::Lock(Path path, std::string owner, Timestamp at)
    : path_(std::move(path)), owner_(std::move(owner)), lockedAt_(at)
{
}

const Path& Lock::path() const
{
    return path_;
}

const std::string& Lock::owner() const
{
    return owner_;
}

Timestamp Lock::lockedAt() const
{
    return lockedAt_;
}

bool Lock::isStale(const Timestamp& now, std::int64_t maxAgeSeconds) const
{
    return lockedAt_.diffSeconds(now) > maxAgeSeconds;
}

}
