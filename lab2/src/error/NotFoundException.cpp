#include "error/NotFoundException.h"

namespace repair
{

NotFoundException::NotFoundException(const std::string& message)
    : RepairException(message) {}

}
