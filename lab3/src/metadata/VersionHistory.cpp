#include "metadata/VersionHistory.h"

#include "error/VersionConflictException.h"

namespace fs
{

VersionHistory::VersionHistory() : maxVersions_(0)
{
}

VersionHistory::VersionHistory(std::size_t maxVersions) : maxVersions_(maxVersions)
{
}

EntryVersion VersionHistory::addVersion(std::string content, Timestamp at)
{
    versions_.emplace_back(static_cast<int>(versions_.size()) + 1, std::move(content), at);
    trimToLimit();
    return versions_.back();
}

void VersionHistory::trimToLimit()
{
    if (maxVersions_ == 0)
    {
        return;
    }
    while (versions_.size() > maxVersions_)
    {
        versions_.erase(versions_.begin());
    }
}

std::size_t VersionHistory::count() const
{
    return versions_.size();
}

const EntryVersion& VersionHistory::latest() const
{
    if (versions_.empty())
    {
        throw VersionConflictException("Version history is empty");
    }
    return versions_.back();
}

const EntryVersion& VersionHistory::findByNumber(int number) const
{
    for (const EntryVersion& version : versions_)
    {
        if (version.number() == number)
        {
            return version;
        }
    }
    throw VersionConflictException("No version #" + std::to_string(number));
}

EntryVersion VersionHistory::rollbackTo(int number, Timestamp at)
{
    const EntryVersion target = findByNumber(number);
    return addVersion(target.content(), at);
}

}
