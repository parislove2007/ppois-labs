#include "entry/BinaryFile.h"

#include "error/PermissionDeniedException.h"

namespace fs
{

BinaryFile::BinaryFile(std::string name, std::string owner, Permission permission,
                       Timestamp created)
    : File(std::move(name), std::move(owner), permission, created),
      mimeType_("application/octet-stream")
{
}

std::string BinaryFile::kind() const
{
    return "binary";
}

const std::string& BinaryFile::mimeType() const
{
    return mimeType_;
}

void BinaryFile::append(const std::string& chunk, Timestamp at)
{
    if (!permission_.allows(Permission::readOnly().withWrite()))
    {
        throw PermissionDeniedException("Write denied: " + name_);
    }
    content_ += chunk;
    checksum_ = Checksum::calculate(content_);
    touch(at);
}

}
