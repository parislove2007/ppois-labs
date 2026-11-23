#pragma once

#include <iosfwd>
#include <string>

#include "value/FileSize.h"
#include "value/Permission.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Базовая запись файловой системы.
 *
 * Абстрактный класс: запись «вообще» не существует — есть файлы,
 * каталоги и символические ссылки. Общие данные (имя, владелец,
 * права, метки времени) живут здесь, размер и описание каждый
 * потомок определяет по-своему.
 */
class FileSystemEntry
{
public:
    /**
     * @brief Создаёт запись.
     * @param name Имя записи без разделителя «/».
     * @param owner Логин владельца.
     * @param permission Права доступа.
     * @param created Момент создания.
     */
    FileSystemEntry(std::string name, std::string owner, Permission permission,
                    Timestamp created);

    virtual ~FileSystemEntry() = default;

    const std::string& name() const;
    const std::string& owner() const;
    const Permission& permission() const;
    Timestamp created() const;
    Timestamp modified() const;

    /** @return Размер записи; каталог считает рекурсивно. */
    virtual FileSize size() const = 0;

    /** @return Человекочитаемое описание записи. */
    virtual std::string describe() const = 0;

    /** @return true для каталогов; переопределяется в Directory. */
    virtual bool isDirectory() const;

    /**
     * @brief Переименовывает запись.
     * @param newName Новое имя без разделителя «/».
     * @param at Момент изменения.
     * @throws InvalidFileNameException Если имя пустое или содержит «/».
     */
    void rename(const std::string& newName, Timestamp at);

    /** @brief Меняет права доступа. */
    void changePermission(Permission permission);

    /** @brief Обновляет метку времени изменения. */
    void touch(Timestamp at);

protected:
    std::string name_;
    std::string owner_;
    Permission permission_;
    Timestamp created_;
    Timestamp modified_;
};

}
