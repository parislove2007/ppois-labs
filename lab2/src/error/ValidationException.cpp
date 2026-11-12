#include "error/ValidationException.h"

namespace repair
{

ValidationException::ValidationException(const std::string& message)
    : RepairException(message) {}

}
