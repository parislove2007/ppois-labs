#include "entry/FileSystemEntry.h"

#include "error/InvalidFileNameException.h"

namespace fs
{

FileSystemEntry::FileSystemEntry(std::string name, std::string owner, Permission permission,
                                 Timestamp created)
    : name_(std::move(name)),
      owner_(std::move(owner)),
      permission_(permission),
      created_(created),
      modified_(created)
{
}

const std::string& FileSystemEntry::name() const
{
    return name_;
}

const std::string& FileSystemEntry::owner() const
{
    return owner_;
}

const Permission& FileSystemEntry::permission() const
{
    return permission_;
}

Timestamp FileSystemEntry::created() const
{
    return created_;
}

Timestamp FileSystemEntry::modified() const
{
    return modified_;
}

bool FileSystemEntry::isDirectory() const
{
    return false;
}

void FileSystemEntry::rename(const std::string& newName, Timestamp at)
{
    if (newName.empty() || newName.find('/') != std::string::npos)
    {
        throw InvalidFileNameException("Invalid entry name: " + newName);
    }
    name_ = newName;
    touch(at);
}

void FileSystemEntry::changePermission(Permission permission)
{
    permission_ = permission;
}

void FileSystemEntry::touch(Timestamp at)
{
    modified_ = at;
}

}
