#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "entry/FileSystemEntry.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Корзина: помнит, откуда удалённая запись пришла.
 *
 * Хранит записи с их исходными путями: восстановление возвращает
 * запись и путь, а не только объект.
 */
class TrashBin
{
public:
    /**
     * @brief Перемещает запись в корзину.
     * @param origin Исходный путь записи.
     * @param entry Сама запись; корзина забирает владение.
     */
    void moveToTrash(Path origin, std::unique_ptr<FileSystemEntry> entry);

    /** @return Количество записей в корзине. */
    std::size_t itemCount() const;

    /**
     * @brief Возвращает запись по индексу.
     * @throws EmptyTrashException Если корзина пуста.
     * @throws PathNotFoundException Если индекс вне диапазона.
     */
    const FileSystemEntry& entryAt(std::size_t index) const;

    /** @return Исходный путь записи по индексу; проверки те же. */
    Path originAt(std::size_t index) const;

    /**
     * @brief Достаёт запись для восстановления.
     * @return Пара «исходный путь + запись».
     */
    std::pair<Path, std::unique_ptr<FileSystemEntry>> restore(std::size_t index);

    /** @brief Очищает корзину. */
    void clear();

private:
    struct Item
    {
        Path origin;
        std::unique_ptr<FileSystemEntry> entry;
    };

    void checkIndex(std::size_t index) const;

    std::vector<Item> items_;
};

}
