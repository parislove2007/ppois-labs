#pragma once

#include <string>
#include <vector>

namespace fs
{

/**
 * @brief Группа пользователей, объединённых по логинам.
 */
class Group
{
public:
    /**
     * @brief Создаёт группу.
     * @param name Имя группы.
     */
    explicit Group(std::string name);

    const std::string& name() const;

    /**
     * @brief Добавляет участника по логину.
     * @throws EntryAlreadyExistsException Если логин уже в группе.
     */
    void addMember(const std::string& login);

    /**
     * @brief Исключает участника по логину.
     * @throws PathNotFoundException Если логина в группе нет.
     */
    void removeMember(const std::string& login);

    bool hasMember(const std::string& login) const;
    std::size_t memberCount() const;

private:
    std::string name_;
    std::vector<std::string> members_;
};

}
