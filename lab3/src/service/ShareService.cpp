#include "service/ShareService.h"

#include <cstdint>

#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"

namespace fs
{

ShareService::ShareService() : nextId_(1)
{
}

std::string ShareService::createShare(const Path& path, const std::string& owner,
                                      Timestamp now, std::int64_t ttlSeconds)
{
    const std::string token = "share-" + std::to_string(nextId_++);
    links_.emplace(token, ShareLink(token, path, owner, now, now.plusSeconds(ttlSeconds)));
    return token;
}

const ShareLink& ShareService::resolve(const std::string& token, Timestamp now) const
{
    auto it = links_.find(token);
    if (it == links_.end() || it->second.isRevoked())
    {
        throw PathNotFoundException("Unknown share token: " + token);
    }
    if (it->second.isExpired(now))
    {
        throw PermissionDeniedException("Share token expired: " + token);
    }
    it->second.registerHit();
    return it->second;
}

void ShareService::revoke(const std::string& token)
{
    auto it = links_.find(token);
    if (it == links_.end() || it->second.isRevoked())
    {
        throw PathNotFoundException("Unknown share token: " + token);
    }
    it->second.revoke();
}

std::size_t ShareService::activeCount() const
{
    return links_.size();
}

}
