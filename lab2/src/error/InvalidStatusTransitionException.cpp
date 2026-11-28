#include "error/InvalidStatusTransitionException.h"

namespace
{
std::string buildMessage(const std::string& fromStatus, const std::string& toStatus)
{
    return "Недопустимый переход заказа из состояния \"" + fromStatus + "\" в состояние \"" + toStatus + "\"";
}
}

namespace repair
{

InvalidStatusTransitionException::InvalidStatusTransitionException(const std::string& fromStatus, const std::string& toStatus)
    : BusinessRuleException(buildMessage(fromStatus, toStatus)),
      fromStatus_(fromStatus),
      toStatus_(toStatus) {}

const std::string& InvalidStatusTransitionException::fromStatus() const { return fromStatus_; }

const std::string& InvalidStatusTransitionException::toStatus() const { return toStatus_; }

}
