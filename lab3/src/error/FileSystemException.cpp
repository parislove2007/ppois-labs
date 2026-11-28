#include "error/FileSystemException.h"

namespace fs
{

FileSystemException::FileSystemException(const std::string& message)
    : std::runtime_error(message)
{
}

}
