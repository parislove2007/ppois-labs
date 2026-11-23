#include "user/AccessControlList.h"

#include "error/PathNotFoundException.h"

namespace fs
{

void AccessControlList::grant(const std::string& login, Permission permission)
{
    rules_[login] = permission;
}

void AccessControlList::revoke(const std::string& login)
{
    auto it = rules_.find(login);
    if (it == rules_.end())
    {
        throw PathNotFoundException("No rule for: " + login);
    }
    rules_.erase(it);
}

bool AccessControlList::hasRuleFor(const std::string& login) const
{
    return rules_.count(login) > 0;
}

Permission AccessControlList::permissionFor(const std::string& login) const
{
    auto it = rules_.find(login);
    if (it == rules_.end())
    {
        throw PathNotFoundException("No rule for: " + login);
    }
    return it->second;
}

bool AccessControlList::allows(const std::string& login, const Permission& required) const
{
    auto it = rules_.find(login);
    return it != rules_.end() && it->second.allows(required);
}

}
