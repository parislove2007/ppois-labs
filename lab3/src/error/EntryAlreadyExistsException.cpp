#include "error/EntryAlreadyExistsException.h"

namespace fs
{

EntryAlreadyExistsException::EntryAlreadyExistsException(const std::string& message)
    : FileSystemException(message)
{
}

}
