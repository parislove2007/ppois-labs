#include "user/User.h"

namespace fs
{

User::User(std::string login, std::string displayName, Timestamp registered)
    : login_(std::move(login)),
      displayName_(std::move(displayName)),
      registered_(registered)
{
}

const std::string& User::login() const
{
    return login_;
}

const std::string& User::displayName() const
{
    return displayName_;
}

Timestamp User::registered() const
{
    return registered_;
}

bool User::canBypassQuota() const
{
    return false;
}

}
