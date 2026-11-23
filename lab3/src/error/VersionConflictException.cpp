#include "error/VersionConflictException.h"

namespace fs
{

VersionConflictException::VersionConflictException(const std::string& message)
    : FileSystemException(message)
{
}

}
