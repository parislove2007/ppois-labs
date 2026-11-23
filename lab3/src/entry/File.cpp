#include "entry/File.h"

#include "error/PermissionDeniedException.h"

namespace fs
{

File::File(std::string name, std::string owner, Permission permission, Timestamp created)
    : FileSystemEntry(std::move(name), std::move(owner), permission, created),
      checksum_(Checksum::calculate(""))
{
}

void File::write(const std::string& content, Timestamp at)
{
    if (!permission_.allows(Permission::readOnly().withWrite()))
    {
        throw PermissionDeniedException("Write denied: " + name_);
    }
    content_ = content;
    checksum_ = Checksum::calculate(content_);
    touch(at);
}

const std::string& File::read() const
{
    return content_;
}

const Checksum& File::checksum() const
{
    return checksum_;
}

FileSize File::size() const
{
    return FileSize::fromBytes(static_cast<std::int64_t>(content_.size()));
}

std::string File::describe() const
{
    return "[" + kind() + "] " + name_ + " (" + std::to_string(content_.size()) + " bytes)";
}

}
