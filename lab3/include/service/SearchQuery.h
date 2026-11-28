#pragma once

#include <memory>
#include <string>

#include "search/SearchFilter.h"
#include "value/FileExtension.h"
#include "value/FileSize.h"
#include "value/Timestamp.h"

namespace fs
{

class Storage;

/**
 * @brief Готовый поисковый запрос: несколько критериев одной настройкой.
 *
 * Пустые строки, нулевые размеры и нулевые метки означают «критерий
 * не задан»; заданные объединяются композитным фильтром «И».
 */
class SearchQuery
{
public:
    SearchQuery();

    SearchQuery& withName(std::string needle, bool caseSensitive = true);
    SearchQuery& withExtension(FileExtension extension);
    SearchQuery& withSizeRange(FileSize min, FileSize max);
    SearchQuery& withModifiedAfter(Timestamp from);
    SearchQuery& withTag(std::string label);

    /** @return true, если задан хотя бы один критерий. */
    bool isEmpty() const;

    /**
     * @brief Собирает фильтр из заданных критериев.
     * @param storage Хранилище для меток.
     * @throws FileSystemException Если критериев нет.
     */
    std::unique_ptr<SearchFilter> buildFilter(const Storage& storage) const;

private:
    std::string needle_;
    bool caseSensitive_;
    FileExtension extension_;
    FileSize minSize_;
    FileSize maxSize_;
    Timestamp modifiedAfter_;
    std::string tag_;
    bool hasName_;
    bool hasExtension_;
    bool hasSize_;
    bool hasTime_;
    bool hasTag_;
};

}
