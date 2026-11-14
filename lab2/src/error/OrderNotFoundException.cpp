#include "error/OrderNotFoundException.h"

namespace
{
std::string buildMessage(int orderNumber)
{
    return "Заказ номер " + std::to_string(orderNumber) + " не найден";
}
}

namespace repair
{

OrderNotFoundException::OrderNotFoundException(int orderNumber)
    : NotFoundException(buildMessage(orderNumber)),
      orderNumber_(orderNumber) {}

int OrderNotFoundException::orderNumber() const { return orderNumber_; }

}
