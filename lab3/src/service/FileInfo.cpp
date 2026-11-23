#include "service/FileInfo.h"

namespace fs
{

FileInfo::FileInfo(Path path, std::string owner, std::string kind, FileSize size,
                   bool isDirectory, Timestamp modified, Permission permission)
    : path_(std::move(path)),
      owner_(std::move(owner)),
      kind_(std::move(kind)),
      size_(size),
      isDirectory_(isDirectory),
      modified_(modified),
      permission_(permission)
{
}

const Path& FileInfo::path() const
{
    return path_;
}

const std::string& FileInfo::owner() const
{
    return owner_;
}

const std::string& FileInfo::kind() const
{
    return kind_;
}

FileSize FileInfo::size() const
{
    return size_;
}

bool FileInfo::isDirectory() const
{
    return isDirectory_;
}

Timestamp FileInfo::modified() const
{
    return modified_;
}

const Permission& FileInfo::permission() const
{
    return permission_;
}

std::string FileInfo::toLine() const
{
    return path_.toString() + " [" + kind_ + "] " + std::to_string(size_.bytes()) + " bytes, "
         + permission_.toString() + ", owner " + owner_;
}

}
