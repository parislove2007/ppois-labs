#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "storage/Storage.h"
#include "value/FileSize.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Отчёты по хранилищу: дерево, сводка и самые большие файлы.
 */
class ReportService
{
public:
    /** @return Дерево хранилища с отступами, строка на запись. */
    std::string treeText(const Storage& storage) const;

    /** @return Сводка: сколько каталогов, файлов, ссылок и байтов. */
    std::string summaryLine(const Storage& storage) const;

    /**
     * @brief Собирает самые большие файлы.
     * @param storage Хранилище.
     * @param count Сколько файлов вернуть.
     * @return Пары «путь — размер», по убыванию размера.
     */
    std::vector<std::pair<Path, FileSize>> topLargest(const Storage& storage,
                                                      std::size_t count) const;
};

}
