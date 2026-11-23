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
    using File::File;

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

private:
    std::vector<std::string> packedNames_;
};

}
