#pragma once

#include "search/SearchFilter.h"
#include "value/FileSize.h"

namespace fs
{

/**
 * @brief Критерий «размер в диапазоне [min, max]».
 */
class SizeFilter : public SearchFilter
{
public:
    SizeFilter(FileSize min, FileSize max);

    bool matches(const Path& path, const FileSystemEntry& entry) const override;

private:
    FileSize min_;
    FileSize max_;
};

}
