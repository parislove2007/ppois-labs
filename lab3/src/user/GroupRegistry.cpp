#include "user/GroupRegistry.h"

#include "error/EntryAlreadyExistsException.h"

namespace fs
{

Group& GroupRegistry::createGroup(const std::string& name)
{
    if (groups_.count(name) > 0)
    {
        throw EntryAlreadyExistsException("Group already exists: " + name);
    }
    groups_.emplace(name, Group(name));
    return groups_.at(name);
}

const Group* GroupRegistry::findByName(const std::string& name) const
{
    auto it = groups_.find(name);
    return it == groups_.end() ? nullptr : &it->second;
}

std::vector<std::string> GroupRegistry::names() const
{
    std::vector<std::string> result;
    for (const auto& entry : groups_)
    {
        result.push_back(entry.first);
    }
    return result;
}

}
