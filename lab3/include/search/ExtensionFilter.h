#pragma once

#include "search/SearchFilter.h"
#include "value/FileExtension.h"

namespace fs
{

/**
 * @brief Критерий «расширение имени равно заданному».
 */
class ExtensionFilter : public SearchFilter
{
public:
    /** @param extension Требуемое расширение. */
    explicit ExtensionFilter(FileExtension extension);

    bool matches(const Path& path, const FileSystemEntry& entry) const override;

private:
    FileExtension extension_;
};

}
