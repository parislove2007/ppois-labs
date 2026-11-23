#include "storage/QuotaManager.h"

#include "error/QuotaExceededException.h"

namespace fs
{

QuotaManager::QuotaManager(FileSize defaultQuota) : defaultQuota_(defaultQuota)
{
}

void QuotaManager::setQuota(const std::string& login, FileSize limit)
{
    quotas_[login] = limit;
}

FileSize QuotaManager::quotaOf(const std::string& login) const
{
    auto it = quotas_.find(login);
    return it == quotas_.end() ? defaultQuota_ : it->second;
}

FileSize QuotaManager::usedOf(const std::string& login) const
{
    auto it = used_.find(login);
    return it == used_.end() ? FileSize() : it->second;
}

void QuotaManager::accountWritten(const std::string& login, FileSize delta)
{
    const FileSize planned = usedOf(login) + delta;
    if (!planned.fitsIn(quotaOf(login)))
    {
        throw QuotaExceededException("Quota exceeded for: " + login);
    }
    used_[login] = planned;
}

void QuotaManager::accountFreed(const std::string& login, FileSize delta)
{
    const FileSize current = usedOf(login);
    used_[login] = delta.fitsIn(current) ? current - delta : FileSize();
}

}
