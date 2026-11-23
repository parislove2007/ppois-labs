#pragma once

#include <memory>
#include <string>
#include <vector>

#include "entry/FileSystemEntry.h"

namespace fs
{

/**
 * @brief Каталог: владеет записями и отвечает за их уникальность.
 *
 * Записи хранятся умными указателями на базовый класс: в одном
 * каталоге лежат файлы разных видов, ссылки и вложенные каталоги —
 * это полиморфный контейнер.
 */
class Directory : public FileSystemEntry
{
public:
    using FileSystemEntry::FileSystemEntry;

    /**
     * @brief Добавляет запись в каталог.
     * @param entry Добавляемая запись; каталог забирает владение.
     * @param at Момент изменения.
     * @throws EntryAlreadyExistsException Если имя уже занято.
     */
    void add(std::unique_ptr<FileSystemEntry> entry, Timestamp at);

    /**
     * @brief Отдаёт запись из каталога (например, в корзину).
     * @param name Имя записи.
     * @param at Момент изменения.
     * @throws PathNotFoundException Если записи с таким именем нет.
     */
    std::unique_ptr<FileSystemEntry> remove(const std::string& name, Timestamp at);

    /**
     * @brief Ищет запись по имени.
     * @throws PathNotFoundException Если записи с таким именем нет.
     */
    FileSystemEntry& find(const std::string& name);

    /** @return true, если запись с таким именем есть. */
    bool contains(const std::string& name) const;

    /** @return Количество записей. */
    std::size_t entryCount() const;

    /** @return Доступ только для чтения к записям каталога. */
    const std::vector<std::unique_ptr<FileSystemEntry>>& entries() const;

    FileSize size() const override;
    std::string describe() const override;
    bool isDirectory() const override;

private:
    std::vector<std::unique_ptr<FileSystemEntry>> entries_;

    std::vector<std::unique_ptr<FileSystemEntry>>::iterator findIterator(
        const std::string& name);
};

}
