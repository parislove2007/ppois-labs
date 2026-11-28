#include "error/StorageFullException.h"

namespace fs
{

StorageFullException::StorageFullException(const std::string& message)
    : FileSystemException(message)
{
}

}
