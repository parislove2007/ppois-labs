#include "user/Group.h"

#include "error/EntryAlreadyExistsException.h"
#include "error/FileSystemException.h"
#include "error/PathNotFoundException.h"

namespace fs
{

Group::Group(std::string name) : Group(std::move(name), "", 0)
{
}

Group::Group(std::string name, std::string ownerLogin, std::size_t maxMembers)
    : name_(std::move(name)), ownerLogin_(std::move(ownerLogin)), maxMembers_(maxMembers)
{
}

const std::string& Group::ownerLogin() const
{
    return ownerLogin_;
}

bool Group::isOwnedBy(const std::string& login) const
{
    return ownerLogin_ == login;
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
    if (maxMembers_ != 0 && members_.size() >= maxMembers_)
    {
        throw FileSystemException("Group is full: " + name_);
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
