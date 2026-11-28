#include "service/ShareLink.h"

namespace fs
{

ShareLink::ShareLink(std::string token, Path path, std::string owner, Timestamp createdAt,
                     Timestamp expiresAt)
    : token_(std::move(token)),
      path_(std::move(path)),
      owner_(std::move(owner)),
      createdAt_(createdAt),
      expiresAt_(expiresAt),
      hits_(0),
      revoked_(false)
{
}

const std::string& ShareLink::token() const
{
    return token_;
}

const Path& ShareLink::path() const
{
    return path_;
}

const std::string& ShareLink::owner() const
{
    return owner_;
}

Timestamp ShareLink::createdAt() const
{
    return createdAt_;
}

Timestamp ShareLink::expiresAt() const
{
    return expiresAt_;
}

bool ShareLink::isExpired(const Timestamp& now) const
{
    return now.isAfter(expiresAt_);
}

void ShareLink::registerHit() const
{
    ++hits_;
}

int ShareLink::hits() const
{
    return hits_;
}

void ShareLink::revoke()
{
    revoked_ = true;
}

bool ShareLink::isRevoked() const
{
    return revoked_;
}

}
