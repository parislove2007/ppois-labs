#pragma once

#include <cstddef>

#include "value/FileSize.h"

namespace fs
{

/**
 * @brief Сводная статистика хранилища: counts по видам записей и total.
 */
class StorageStats
{
public:
    StorageStats();

    void countDirectory();
    void countFile();
    void countLink();
    void addBytes(FileSize size);

    std::size_t directories() const;
    std::size_t files() const;
    std::size_t links() const;
    FileSize totalBytes() const;

    /** @return Строка сводки для отчёта. */
    std::string toLine() const;

private:
    std::size_t directories_;
    std::size_t files_;
    std::size_t links_;
    FileSize totalBytes_;
};

}
