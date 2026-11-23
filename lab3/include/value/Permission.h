#pragma once

#include <iosfwd>
#include <string>

namespace fs
{

/**
 * @brief Права доступа к записи: чтение, запись, выполнение.
 *
 * Неизменяемый объект-значение. Новые права строятся методами
 * withRead/withWrite/withExecute, исходный объект не меняется.
 */
class Permission
{
public:
    /** @brief Создаёт пустые права. */
    Permission();

    static Permission none();
    static Permission readOnly();
    static Permission readWrite();
    static Permission full();

    bool canRead() const;
    bool canWrite() const;
    bool canExecute() const;

    Permission withRead() const;
    Permission withWrite() const;
    Permission withExecute() const;

    /**
     * @brief Проверяет, что текущих прав достаточно.
     * @param required Требуемый набор прав.
     * @return true, если все требуемые биты присутствуют.
     */
    bool allows(const Permission& required) const;

    /** @return Строковое представление в стиле «rwx». */
    std::string toString() const;

    bool operator==(const Permission& other) const;

private:
    Permission(bool read, bool write, bool execute);

    bool read_;
    bool write_;
    bool execute_;
};

std::ostream& operator<<(std::ostream& out, const Permission& permission);

}
