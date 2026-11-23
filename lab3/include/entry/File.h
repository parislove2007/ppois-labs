#pragma once

#include <string>

#include "entry/FileSystemEntry.h"
#include "value/Checksum.h"

namespace fs
{

/**
 * @brief Файл с содержимым и контрольной суммой.
 *
 * Промежуточный абстрактный класс: общий механизм хранения и записи
 * содержимого один, а вид файла (текст, бинарный, архив) определяют
 * наследники через kind().
 */
class File : public FileSystemEntry
{
public:
    /**
     * @brief Создаёт пустой файл.
     * @param name Имя файла.
     * @param owner Логин владельца.
     * @param permission Права доступа.
     * @param created Момент создания.
     */
    File(std::string name, std::string owner, Permission permission, Timestamp created);

    /** @return Вид файла: «text», «binary» или «archive». */
    virtual std::string kind() const = 0;

    /**
     * @brief Полностью заменяет содержимое файла.
     * @param content Новое содержимое.
     * @param at Момент изменения.
     * @throws PermissionDeniedException Если нет права записи.
     */
    void write(const std::string& content, Timestamp at);

    /** @return Текущее содержимое. */
    const std::string& read() const;

    /** @return Контрольная сумма текущего содержимого. */
    const Checksum& checksum() const;

    FileSize size() const override;

    std::string describe() const override;

protected:
    std::string content_;
    Checksum checksum_;
};

}
