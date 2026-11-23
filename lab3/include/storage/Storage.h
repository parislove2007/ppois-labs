#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "entry/Directory.h"
#include "entry/FileSystemEntry.h"
#include "metadata/FileMetadata.h"
#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Хранилище: дерево каталогов и индекс метаданных по путям.
 *
 * Единственный владелец дерева записей: каталоги и файлы живут
 * только внутри него, наружу отдаются по ссылке или через detach.
 */
class Storage
{
public:
    /** @brief Создаёт хранилище с пустым корнем «/». */
    Storage();

    /** @return Корневой каталог. */
    Directory& root();

    /** @return Корневой каталог для чтения. */
    const Directory& root() const;

    /**
     * @brief Находит запись по полному пути.
     * @throws PathNotFoundException Если сегмент не находится.
     * @throws NotADirectoryException Если промежуточный сегмент — не каталог.
     */
    FileSystemEntry& resolve(const Path& path);

    /**
     * @brief Находит или создаёт каталог по полному пути.
     * @throws EntryAlreadyExistsException Если на пути встретился файл.
     */
    Directory& ensureDirectory(const Path& path, const std::string& owner, Timestamp at);

    /**
     * @brief Кладёт запись по полному пути в существующий каталог.
     * @throws PathNotFoundException Если родительский каталог не существует.
     * @throws EntryAlreadyExistsException Если имя занято.
     */
    void attach(const Path& path, std::unique_ptr<FileSystemEntry> entry, Timestamp at);

    /**
     * @brief Забирает запись из дерева (например, в корзину).
     * @throws PathNotFoundException Если записи нет.
     * @throws FileSystemException Если пытаются detached корень.
     */
    std::unique_ptr<FileSystemEntry> detach(const Path& path, Timestamp at);

    /**
     * @brief Возвращает метаданные пути, при отсутствии создаёт их.
     */
    FileMetadata& metadataFor(const Path& path);

    /** @return true, если у пути есть метаданные. */
    bool hasMetadata(const Path& path) const;

    /** @return Метаданные пути или nullptr, если их ещё нет. */
    const FileMetadata* findMetadata(const Path& path) const;

    /**
     * @brief Обходит все записи дерева, включая корень и вложенные каталоги.
     * @param visitor Функция, принимающая путь и запись.
     */
    void walk(const std::function<void(const Path&, const FileSystemEntry&)>& visitor) const;

private:
    Directory& parentDirectoryFor(const Path& path);
    static void walkFrom(const Path& path, const Directory& directory,
                         const std::function<void(const Path&, const FileSystemEntry&)>& visitor);

    Directory root_;
    std::map<std::string, FileMetadata> metadata_;
};

}
