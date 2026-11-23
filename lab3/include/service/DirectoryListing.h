#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "value/FileSize.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Готовый листинг каталога: имена, счётчики и суммарный размер.
 */
class DirectoryListing
{
public:
    DirectoryListing(Path path, std::vector<std::string> entryNames, std::size_t fileCount,
                     std::size_t directoryCount, FileSize totalBytes);

    const Path& path() const;
    const std::vector<std::string>& entryNames() const;
    std::size_t fileCount() const;
    std::size_t directoryCount() const;
    FileSize totalBytes() const;

    /** @return Строка листинга: путь и имена через пробел. */
    std::string toLine() const;

private:
    Path path_;
    std::vector<std::string> entryNames_;
    std::size_t fileCount_;
    std::size_t directoryCount_;
    FileSize totalBytes_;
};

}
