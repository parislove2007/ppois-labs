#include "error/InvalidPhoneNumberException.h"

namespace
{
std::string buildMessage(const std::string& rawNumber)
{
    return "Телефонный номер не соответствует формату: " + rawNumber;
}
}

namespace repair
{

InvalidPhoneNumberException::InvalidPhoneNumberException(const std::string& rawNumber)
    : ValidationException(buildMessage(rawNumber)),
      rawNumber_(rawNumber) {}

const std::string& InvalidPhoneNumberException::rawNumber() const { return rawNumber_; }

}
