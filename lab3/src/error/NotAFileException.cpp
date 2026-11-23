#include "error/NotAFileException.h"

namespace fs
{

NotAFileException::NotAFileException(const std::string& message)
    : FileSystemException(message)
{
}

}
