#include "entry/ArchiveFile.h"

#include <algorithm>

#include "error/EntryAlreadyExistsException.h"
#include "error/FileSystemException.h"

namespace fs
{

ArchiveFile::ArchiveFile(std::string name, std::string owner, Permission permission,
                         Timestamp created)
    : File(std::move(name), std::move(owner), permission, created), compressionLevel_(6)
{
}

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

int ArchiveFile::compressionLevel() const
{
    return compressionLevel_;
}

void ArchiveFile::setCompressionLevel(int level)
{
    if (level < 1 || level > 9)
    {
        throw FileSystemException("Compression level must be 1..9");
    }
    compressionLevel_ = level;
}

const std::string& ArchiveFile::comment() const
{
    return comment_;
}

void ArchiveFile::setComment(std::string comment)
{
    comment_ = std::move(comment);
}

std::size_t ArchiveFile::packedCount() const
{
    return packedNames_.size();
}

bool ArchiveFile::containsPacked(const std::string& entryName) const
{
    return std::any_of(packedNames_.begin(), packedNames_.end(),
                       [&entryName](const std::string& packed) {
                           return packed == entryName;
                       });
}

}
