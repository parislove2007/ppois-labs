#pragma once

#include <string>

#include "search/SearchFilter.h"

namespace fs
{

/**
 * @brief Критерий «имя содержит подстроку».
 */
class NameFilter : public SearchFilter
{
public:
    /** @param needle Подстрока для поиска в имени. */
    explicit NameFilter(std::string needle);

    /**
     * @param needle Подстрока для поиска в имени.
     * @param caseSensitive Учитывать ли регистр.
     */
    NameFilter(std::string needle, bool caseSensitive);

    bool matches(const Path& path, const FileSystemEntry& entry) const override;

private:
    std::string needle_;
    bool caseSensitive_;
};

}
