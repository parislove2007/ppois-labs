#include "user/User.h"

namespace fs
{

User::User(std::string login, std::string displayName, Timestamp registered,
           std::string email, std::string department)
    : login_(std::move(login)),
      displayName_(std::move(displayName)),
      registered_(registered),
      email_(std::move(email)),
      department_(std::move(department)),
      isActive_(true)
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

const std::string& User::email() const
{
    return email_;
}

const std::string& User::department() const
{
    return department_;
}

void User::deactivate()
{
    isActive_ = false;
}

void User::activate()
{
    isActive_ = true;
}

bool User::isActive() const
{
    return isActive_;
}

Timestamp User::lastLoginAt() const
{
    return lastLoginAt_;
}

void User::markLoggedIn(Timestamp at)
{
    lastLoginAt_ = at;
}

bool User::canBypassQuota() const
{
    return false;
}

}
