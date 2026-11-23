#include "error/FileLockedException.h"

namespace fs
{

FileLockedException::FileLockedException(const std::string& message)
    : FileSystemException(message)
{
}

}
