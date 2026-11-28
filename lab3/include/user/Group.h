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
     * @brief Создаёт группу без ограничений.
     * @param name Имя группы.
     */
    explicit Group(std::string name);

    /**
     * @brief Создаёт группу с владельцем и лимитом участников.
     * @param name Имя группы.
     * @param ownerLogin Логин владельца группы.
     * @param maxMembers Максимум участников; ноль — без лимита.
     */
    Group(std::string name, std::string ownerLogin, std::size_t maxMembers);

    const std::string& name() const;

    /** @return Логин владельца группы. */
    const std::string& ownerLogin() const;

    /** @brief Проверяет, что логин — владелец группы. */
    bool isOwnedBy(const std::string& login) const;

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
    std::string ownerLogin_;
    std::size_t maxMembers_;
    std::vector<std::string> members_;
};

}
