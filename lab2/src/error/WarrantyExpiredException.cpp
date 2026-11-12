#include "error/WarrantyExpiredException.h"

namespace
{
std::string buildMessage(int orderNumber, const std::string& expiryDate)
{
    return "Гарантия по заказу номер " + std::to_string(orderNumber)
           + " истекла " + expiryDate;
}
}

namespace repair
{

WarrantyExpiredException::WarrantyExpiredException(int orderNumber, const std::string& expiryDate)
    : BusinessRuleException(buildMessage(orderNumber, expiryDate)),
      orderNumber_(orderNumber),
      expiryDate_(expiryDate) {}

int WarrantyExpiredException::orderNumber() const { return orderNumber_; }

const std::string& WarrantyExpiredException::expiryDate() const { return expiryDate_; }

}
