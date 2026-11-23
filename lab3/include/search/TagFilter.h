#pragma once

#include <string>

#include "search/SearchFilter.h"
#include "storage/Storage.h"

namespace fs
{

/**
 * @brief Критерий «на записи стоит метка из метаданных хранилища».
 */
class TagFilter : public SearchFilter
{
public:
    /**
     * @brief Создаёт критерий по метке.
     * @param storage Хранилище, в метаданных которого ищем метки.
     * @param label Текст метки.
     */
    TagFilter(const Storage& storage, std::string label);

    bool matches(const Path& path, const FileSystemEntry& entry) const override;

private:
    const Storage& storage_;
    std::string label_;
};

}
