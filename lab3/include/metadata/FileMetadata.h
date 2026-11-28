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

    const std::string& author() const;
    void setAuthor(std::string author);

    const std::string& language() const;
    void setLanguage(std::string language);

    const std::string& createdBy() const;
    void setCreatedBy(std::string login);

    /** @brief Переключает признак «избранное». */
    void toggleFavorite();
    bool isFavorite() const;

    /**
     * @brief Ставит оценку от нуля до пяти.
     * @throws FileSystemException Если оценка вне диапазона.
     */
    void setRating(int rating);
    int rating() const;

private:
    std::string description_;
    std::vector<Tag> tags_;
    std::string author_;
    std::string language_;
    std::string createdBy_;
    bool isFavorite_;
    int rating_;
};

}
