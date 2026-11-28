#include "error/PermissionDeniedException.h"

namespace fs
{

PermissionDeniedException::PermissionDeniedException(const std::string& message)
    : FileSystemException(message)
{
}

}
