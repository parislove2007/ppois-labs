#include "error/InvalidFileSizeException.h"

namespace fs
{

InvalidFileSizeException::InvalidFileSizeException(const std::string& message)
    : FileSystemException(message)
{
}

}
