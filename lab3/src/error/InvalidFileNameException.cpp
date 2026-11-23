#include "error/InvalidFileNameException.h"

namespace fs
{

InvalidFileNameException::InvalidFileNameException(const std::string& message)
    : FileSystemException(message)
{
}

}
