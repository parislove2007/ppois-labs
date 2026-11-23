#include "entry/ArchiveFile.h"

#include "error/EntryAlreadyExistsException.h"

namespace fs
{

std::string ArchiveFile::kind() const
{
    return "archive";
}

void ArchiveFile::packEntry(const std::string& entryName, Timestamp at)
{
    if (containsPacked(entryName))
    {
        throw EntryAlreadyExistsException("Already packed: " + entryName);
    }
    packedNames_.push_back(entryName);
    touch(at);
}

std::size_t ArchiveFile::packedCount() const
{
    return packedNames_.size();
}

bool ArchiveFile::containsPacked(const std::string& entryName) const
{
    for (const std::string& packed : packedNames_)
    {
        if (packed == entryName)
        {
            return true;
        }
    }
    return false;
}

}
