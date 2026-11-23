#include "user/Group.h"

#include "error/EntryAlreadyExistsException.h"
#include "error/PathNotFoundException.h"

namespace fs
{

Group::Group(std::string name) : name_(std::move(name))
{
}

const std::string& Group::name() const
{
    return name_;
}

void Group::addMember(const std::string& login)
{
    if (hasMember(login))
    {
        throw EntryAlreadyExistsException("Already in group: " + login);
    }
    members_.push_back(login);
}

void Group::removeMember(const std::string& login)
{
    for (auto it = members_.begin(); it != members_.end(); ++it)
    {
        if (*it == login)
        {
            members_.erase(it);
            return;
        }
    }
    throw PathNotFoundException("Not in group: " + login);
}

bool Group::hasMember(const std::string& login) const
{
    for (const std::string& member : members_)
    {
        if (member == login)
        {
            return true;
        }
    }
    return false;
}

std::size_t Group::memberCount() const
{
    return members_.size();
}

}
