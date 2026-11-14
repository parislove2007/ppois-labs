#include "error/InvalidSerialNumberException.h"

namespace
{
std::string buildMessage(const std::string& rawSerial)
{
    return "Серийный номер не соответствует формату: " + rawSerial;
}
}

namespace repair
{

InvalidSerialNumberException::InvalidSerialNumberException(const std::string& rawSerial)
    : ValidationException(buildMessage(rawSerial)),
      rawSerial_(rawSerial) {}

const std::string& InvalidSerialNumberException::rawSerial() const { return rawSerial_; }

}
