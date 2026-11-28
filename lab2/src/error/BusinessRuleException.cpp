#include "error/BusinessRuleException.h"

namespace repair
{

BusinessRuleException::BusinessRuleException(const std::string& message)
    : RepairException(message) {}

}
