#include "error/ClientNotFoundException.h"

namespace
{
std::string buildMessage(const std::string& phone)
{
    return "Клиент с телефоном " + phone + " не зарегистрирован";
}
}

namespace repair
{

ClientNotFoundException::ClientNotFoundException(const std::string& phone)
    : NotFoundException(buildMessage(phone)),
      phone_(phone) {}

const std::string& ClientNotFoundException::phone() const { return phone_; }

}
