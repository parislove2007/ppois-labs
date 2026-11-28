#pragma once

#include <memory>
#include <vector>

#include "search/SearchFilter.h"

namespace fs
{

/**
 * @brief Комбинация критериев: «все одновременно» или «хотя бы один».
 */
class CompositeFilter : public SearchFilter
{
public:
    /**
     * @brief Создаёт комбинированный критерий.
     * @param matchAll true — логическое И, false — логическое ИЛИ.
     */
    explicit CompositeFilter(bool matchAll);

    /** @brief Добавляет вложенный критерий; комбинатор забирает владение. */
    void add(std::unique_ptr<SearchFilter> filter);

    bool matches(const Path& path, const FileSystemEntry& entry) const override;

private:
    bool matchAll_;
    std::vector<std::unique_ptr<SearchFilter>> filters_;
};

}
