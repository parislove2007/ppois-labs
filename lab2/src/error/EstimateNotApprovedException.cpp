#include "error/EstimateNotApprovedException.h"

namespace
{
std::string buildMessage(int orderNumber)
{
    return "Смета по заказу номер " + std::to_string(orderNumber) + " не утверждена клиентом";
}
}

namespace repair
{

EstimateNotApprovedException::EstimateNotApprovedException(int orderNumber)
    : BusinessRuleException(buildMessage(orderNumber)),
      orderNumber_(orderNumber) {}

int EstimateNotApprovedException::orderNumber() const { return orderNumber_; }

}
