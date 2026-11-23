#include "entry/BinaryFile.h"

#include "error/PermissionDeniedException.h"

namespace fs
{

std::string BinaryFile::kind() const
{
    return "binary";
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
