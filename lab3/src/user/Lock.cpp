#include "user/Lock.h"

namespace fs
{

Lock::Lock(Path path, std::string owner, Timestamp at, std::string comment)
    : path_(std::move(path)),
      owner_(std::move(owner)),
      lockedAt_(at),
      comment_(std::move(comment))
{
}

const std::string& Lock::comment() const
{
    return comment_;
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
    return now.diffSeconds(lockedAt_) > maxAgeSeconds;
}

}
