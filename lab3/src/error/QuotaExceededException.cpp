#include "error/QuotaExceededException.h"

namespace fs
{

QuotaExceededException::QuotaExceededException(const std::string& message)
    : FileSystemException(message)
{
}

}
