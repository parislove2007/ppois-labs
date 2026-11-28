#include "error/RepairException.h"

namespace repair
{

RepairException::RepairException(const std::string& message)
    : std::runtime_error(message) {}

}
