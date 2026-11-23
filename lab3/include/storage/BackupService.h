#pragma once

#include <string>
#include <vector>

#include "storage/Storage.h"
#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Одна сохранённая копия файла: путь и содержимое.
 */
class BackupRecord
{
public:
    BackupRecord(Path path, std::string content);

    const Path& path() const;
    const std::string& content() const;

private:
    Path path_;
    std::string content_;
};

/**
 * @brief Резервное копирование содержимого всех файлов хранилища.
 *
 * Снимок хранит содержимое файлов; символические ссылки и каталоги
 * не копируются. Восстановление перезаписывает существующие файлы,
 * у которых есть право записи.
 */
class BackupService
{
public:
    /**
     * @brief Снимает копию всех файлов хранилища.
     * @param storage Хранилище-источник.
     */
    void capture(const Storage& storage);

    /** @return Количество записей в снимке. */
    std::size_t recordCount() const;

    bool has(const Path& path) const;

    /**
     * @brief Возвращает содержимое по пути.
     * @throws PathNotFoundException Если пути нет в снимке.
     */
    std::string contentOf(const Path& path) const;

    /**
     * @brief Восстанавливает снимок в хранилище.
     * @param storage Хранилище-приёмник.
     * @param at Момент восстановления.
     * @return Число перезаписанных файлов.
     */
    std::size_t restoreTo(Storage& storage, Timestamp at) const;

private:
    std::vector<BackupRecord> records_;
};

}
