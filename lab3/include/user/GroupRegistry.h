#pragma once

#include <map>
#include <string>
#include <vector>

#include "user/Group.h"

namespace fs
{

/**
 * @brief Каталог групп: создание и поиск по имени.
 */
class GroupRegistry
{
public:
    /**
     * @brief Создаёт группу.
     * @throws EntryAlreadyExistsException Если имя занято.
     */
    Group& createGroup(const std::string& name);

    /** @return Группа по имени или nullptr. */
    const Group* findByName(const std::string& name) const;

    /** @return Имена всех групп. */
    std::vector<std::string> names() const;

private:
    std::map<std::string, Group> groups_;
};

}
