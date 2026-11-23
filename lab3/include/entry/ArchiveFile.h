#pragma once

#include <string>
#include <vector>

#include "entry/File.h"

namespace fs
{

/**
 * @brief Архивный файл: помнит имена упакованных записей.
 */
class ArchiveFile : public File
{
public:
    ArchiveFile(std::string name, std::string owner, Permission permission,
                Timestamp created);

    std::string kind() const override;

    /**
     * @brief Упаковывает запись по имени в архив.
     * @param entryName Имя упаковываемой записи.
     * @param at Момент изменения.
     * @throws EntryAlreadyExistsException Если имя уже упаковано.
     */
    void packEntry(const std::string& entryName, Timestamp at);

    /** @return Количество упакованных записей. */
    std::size_t packedCount() const;

    /** @return true, если запись с таким именем уже в архиве. */
    bool containsPacked(const std::string& entryName) const;

    /** @return Уровень сжатия от одного до девяти. */
    int compressionLevel() const;

    /**
     * @brief Меняет уровень сжатия.
     * @throws FileSystemException Если уровень вне диапазона 1–9.
     */
    void setCompressionLevel(int level);

    const std::string& comment() const;
    void setComment(std::string comment);

private:
    std::vector<std::string> packedNames_;
    int compressionLevel_;
    std::string comment_;
};

}
