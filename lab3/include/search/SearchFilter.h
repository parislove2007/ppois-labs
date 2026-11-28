#pragma once

#include "entry/FileSystemEntry.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Абстрактный критерий поиска.
 *
 * Фильтр получает и путь, и запись: путь нужен критериям, которые
 * смотрят внешние данные (например, метки в метаданных хранилища).
 */
class SearchFilter
{
public:
    virtual ~SearchFilter() = default;

    /**
     * @brief Проверяет, подходит ли запись под критерий.
     * @param path Полный путь записи.
     * @param entry Сама запись.
     * @return true, если запись подходит.
     */
    virtual bool matches(const Path& path, const FileSystemEntry& entry) const = 0;
};

}
