#include "error/PathNotFoundException.h"

namespace fs
{

PathNotFoundException::PathNotFoundException(const std::string& message)
    : FileSystemException(message)
{
}

}
