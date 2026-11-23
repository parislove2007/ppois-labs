#include "user/UserRegistry.h"

#include "error/EntryAlreadyExistsException.h"
#include "error/PermissionDeniedException.h"
#include "value/Checksum.h"

namespace fs
{

UserRegistry::UserRegistry(std::size_t maxFailedAttempts)
    : maxFailedAttempts_(maxFailedAttempts)
{
}

void UserRegistry::registerUser(std::unique_ptr<User> user, const std::string& password)
{
    const std::string login = user->login();
    if (users_.count(login) > 0)
    {
        throw EntryAlreadyExistsException("Login already taken: " + login);
    }
    Credentials credentials;
    credentials.passwordHash_ = Checksum::calculate(password).value();
    credentials_[login] = credentials;
    users_[login] = std::move(user);
}

User* UserRegistry::findByLogin(const std::string& login) const
{
    auto it = users_.find(login);
    return it == users_.end() ? nullptr : it->second.get();
}

User& UserRegistry::authenticate(const std::string& login, const std::string& password)
{
    auto userIt = users_.find(login);
    if (userIt == users_.end())
    {
        throw PermissionDeniedException("Unknown login: " + login);
    }
    Credentials& credentials = credentials_[login];
    if (credentials.blocked_)
    {
        throw PermissionDeniedException("Account is blocked: " + login);
    }
    if (Checksum::calculate(password).value() != credentials.passwordHash_)
    {
        ++credentials.failedAttempts_;
        if (credentials.failedAttempts_ >= maxFailedAttempts_)
        {
            credentials.blocked_ = true;
        }
        throw PermissionDeniedException("Wrong password for: " + login);
    }
    credentials.failedAttempts_ = 0;
    return *userIt->second;
}

std::vector<std::string> UserRegistry::logins() const
{
    std::vector<std::string> result;
    for (const auto& entry : users_)
    {
        result.push_back(entry.first);
    }
    return result;
}

}
