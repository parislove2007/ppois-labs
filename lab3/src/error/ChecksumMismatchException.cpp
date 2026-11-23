#include "error/ChecksumMismatchException.h"

namespace fs
{

ChecksumMismatchException::ChecksumMismatchException(const std::string& message)
    : FileSystemException(message)
{
}

}
