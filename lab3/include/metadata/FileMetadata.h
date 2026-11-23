#pragma once

#include <string>
#include <vector>

#include "metadata/Tag.h"

namespace fs
{

/**
 * @brief Описательные метаданные файла: текстовое описание и метки.
 */
class FileMetadata
{
public:
    /** @brief Создаёт пустые метаданные. */
    FileMetadata();

    const std::string& description() const;
    void setDescription(std::string description);

    /**
     * @brief Добавляет метку.
     * @throws EntryAlreadyExistsException Если метка уже стоит.
     */
    void addTag(Tag tag);

    /**
     * @brief Снимает метку по тексту.
     * @throws PathNotFoundException Если такой метки нет.
     */
    void removeTag(const std::string& label);

    /** @return true, если метка стоит. */
    bool hasTag(const std::string& label) const;

    /** @return Количество меток. */
    std::size_t tagCount() const;

private:
    std::string description_;
    std::vector<Tag> tags_;
};

}
