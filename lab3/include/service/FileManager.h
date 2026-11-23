#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "metadata/Tag.h"
#include "metadata/VersionHistory.h"
#include "operations/OperationJournal.h"
#include "search/SearchFilter.h"
#include "storage/BackupService.h"
#include "storage/QuotaManager.h"
#include "storage/Storage.h"
#include "storage/TrashBin.h"
#include "user/User.h"
#include "value/FileSize.h"
#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Фасад файловой системы: единая точка входа для всех операций.
 *
 * Владеет хранилищем, квотами, корзиной, журналом, версиями и резервными
 * копиями. CLI и отчёты работают только через него — доменная логика
 * не знает ни о консоли, ни о потоках ввода.
 */
class FileManager
{
public:
    /**
     * @brief Создаёт файловую систему.
     * @param defaultQuota Квота по умолчанию для новых пользователей.
     */
    explicit FileManager(FileSize defaultQuota);

    const Storage& storage() const;

    /** @return Хранилище для изменений. */
    Storage& storage();

    const QuotaManager& quotas() const;
    const TrashBin& trash() const;
    const OperationJournal& journal() const;

    /** @brief Создаёт каталог по пути, включая промежуточные. */
    void createDirectory(const Path& path, const User& owner, Timestamp at);

    /**
     * @brief Создаёт файл с содержимым.
     *
     * Вид файла выбирается по расширению: текстовое — TextFile,
     * иначе BinaryFile. Файл создаётся с правом записи — иначе его
     * нельзя было бы наполнить.
     * @throws QuotaExceededException Если владелец превысил квоту.
     */
    void createFile(const Path& path, const std::string& content, const User& owner,
                    Timestamp at);

    /**
     * @brief Перезаписывает содержимое файла.
     * @throws NotAFileException Если по пути не файл.
     * @throws PermissionDeniedException Если у записи нет права записи.
     * @throws QuotaExceededException Если новый размер не влезает в квоту.
     */
    void write(const Path& path, const std::string& content, Timestamp at);

    /** @brief Перезаписывает файл и сохраняет версию в историю. */
    void saveVersion(const Path& path, const std::string& content, Timestamp at);

    /** @return История версий пути; создаётся при первом обращении. */
    VersionHistory& versionHistoryFor(const Path& path);

    /**
     * @brief Откатывает файл к версии: содержимое версии перезаписывается.
     * @throws VersionConflictException Если версии с таким номером нет.
     */
    void rollback(const Path& path, int versionNumber, Timestamp at);

    /** @brief Удаляет запись в корзину. */
    void remove(const Path& path, Timestamp at);

    /** @brief Восстанавливает запись из корзины по индексу. */
    void restoreFromTrash(std::size_t index, Timestamp at);

    /** @brief Перемещает запись по новому пути. */
    void move(const Path& from, const Path& to, Timestamp at);

    /**
     * @brief Копирует файл по новому пути.
     * @throws NotAFileException Если источник — каталог или ссылка.
     */
    void copyFile(const Path& from, const Path& to, Timestamp at);

    /** @brief Переименовывает запись. */
    void rename(const Path& path, const std::string& newName, Timestamp at);

    /** @brief Ставит метку на путь. */
    void tag(const Path& path, Tag tag);

    /** @brief Задаёт текстовое описание пути. */
    void describe(const Path& path, std::string description);

    /** @return Пути записей, подходящих под критерий. */
    std::vector<Path> search(const SearchFilter& filter) const;

    /** @brief Снимает резервную копию всех файлов. */
    void makeBackup();

    /**
     * @brief Восстанавливает содержимое файлов из последней копии.
     * @return Число перезаписанных файлов.
     */
    std::size_t restoreBackup(Timestamp at);

private:
    void chargeQuotaDelta(const std::string& login, std::uint64_t oldBytes,
                          std::uint64_t newBytes);

    Storage storage_;
    QuotaManager quotas_;
    TrashBin trash_;
    OperationJournal journal_;
    BackupService backup_;
    std::map<std::string, VersionHistory> histories_;
};

}
