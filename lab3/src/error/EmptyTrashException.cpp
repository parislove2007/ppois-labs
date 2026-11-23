#include "error/EmptyTrashException.h"

namespace fs
{

EmptyTrashException::EmptyTrashException(const std::string& message)
    : FileSystemException(message)
{
}

}
