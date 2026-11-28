#pragma once

#include <vector>

#include "search/SearchFilter.h"
#include "storage/Storage.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Поиск по хранилищу с любым критерием.
 */
class SearchService
{
public:
    /**
     * @brief Ищет все записи, подходящие под критерий.
     * @param storage Хранилище для поиска.
     * @param filter Критерий.
     * @return Пути подходящих записей в порядке обхода дерева.
     */
    std::vector<Path> search(const Storage& storage, const SearchFilter& filter) const;
};

}
