#include "metadata/VersionHistory.h"

#include "error/VersionConflictException.h"

namespace fs
{

VersionHistory::VersionHistory()
{
}

EntryVersion VersionHistory::addVersion(std::string content, Timestamp at)
{
    versions_.emplace_back(static_cast<int>(versions_.size()) + 1, std::move(content), at);
    return versions_.back();
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
