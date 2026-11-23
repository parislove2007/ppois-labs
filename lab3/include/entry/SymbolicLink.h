#pragma once

#include <string>

#include "entry/FileSystemEntry.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Символическая ссылка: указывает на другой путь.
 */
class SymbolicLink : public FileSystemEntry
{
public:
    /**
     * @brief Создаёт ссылку.
     * @param name Имя ссылки.
     * @param target Путь, на который ссылка указывает.
     */
    SymbolicLink(std::string name, Path target, std::string owner, Permission permission,
                 Timestamp created);

    /** @return Путь-цель ссылки. */
    const Path& target() const;

    /**
     * @brief Перенаправляет ссылку на новый путь.
     * @param newTarget Новый путь-цель.
     * @param at Момент изменения.
     */
    void retarget(Path newTarget, Timestamp at);

    FileSize size() const override;
    std::string describe() const override;

private:
    Path target_;
};

}
