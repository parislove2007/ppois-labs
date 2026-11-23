#pragma once

#include <cstdint>
#include <string>

#include "value/FileSize.h"
#include "value/Path.h"
#include "value/Permission.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Снимок данных записи для отчётов и поиска.
 *
 * Копия только нужных полей: отчёты работают с FileInfo и не зависят
 * от устройства самих записей.
 */
class FileInfo
{
public:
    FileInfo(Path path, std::string owner, std::string kind, FileSize size,
             bool isDirectory, Timestamp modified, Permission permission);

    const Path& path() const;
    const std::string& owner() const;
    const std::string& kind() const;
    FileSize size() const;
    bool isDirectory() const;
    Timestamp modified() const;
    const Permission& permission() const;

    /** @return Строка отчёта: путь, вид, размер. */
    std::string toLine() const;

private:
    Path path_;
    std::string owner_;
    std::string kind_;
    FileSize size_;
    bool isDirectory_;
    Timestamp modified_;
    Permission permission_;
};

}
