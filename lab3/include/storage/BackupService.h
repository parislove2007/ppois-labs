#pragma once

#include <string>
#include <vector>

#include "storage/Storage.h"
#include "value/Checksum.h"
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

    /** @return Контрольная сумма сохранённого содержимого. */
    const Checksum& checksum() const;

    /** @return true, если переданное содержимое совпадает с сохранённым. */
    bool verify(const std::string& content) const;

private:
    Path path_;
    std::string content_;
    Checksum checksum_;
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
    /** @brief Создаёт пустой снимок без метки. */
    BackupService();

    /**
     * @brief Снимает копию всех файлов хранилища.
     * @param storage Хранилище-источник.
     */
    void capture(const Storage& storage);

    /**
     * @brief Снимает копию с меткой и моментом создания.
     * @param label Метка снимка, например «перед чисткой».
     * @param createdAt Момент создания снимка.
     */
    void capture(const Storage& storage, std::string label, Timestamp createdAt);

    const std::string& label() const;
    Timestamp createdAt() const;

    /** @brief Помечает копию как зашифрованную. */
    void toggleEncryption();
    bool isEncrypted() const;

    /** @return Описание снимка: метка, момент и число записей. */
    std::string describe() const;

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
    std::string label_;
    Timestamp createdAt_;
    bool isEncrypted_;
};

}
