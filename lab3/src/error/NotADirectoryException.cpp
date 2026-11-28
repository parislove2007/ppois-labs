#include "error/NotADirectoryException.h"

namespace fs
{

NotADirectoryException::NotADirectoryException(const std::string& message)
    : FileSystemException(message)
{
}

}
