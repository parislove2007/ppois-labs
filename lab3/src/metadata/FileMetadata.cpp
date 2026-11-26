#include "metadata/FileMetadata.h"

#include <algorithm>

#include "error/EntryAlreadyExistsException.h"
#include "error/FileSystemException.h"
#include "error/PathNotFoundException.h"

namespace fs
{

FileMetadata::FileMetadata()
    : language_("en"), isFavorite_(false), rating_(0)
{
}

const std::string& FileMetadata::description() const
{
    return description_;
}

void FileMetadata::setDescription(std::string description)
{
    description_ = std::move(description);
}

void FileMetadata::addTag(Tag tag)
{
    if (hasTag(tag.label()))
    {
        throw EntryAlreadyExistsException("Tag already set: " + tag.label());
    }
    tags_.push_back(std::move(tag));
}

void FileMetadata::removeTag(const std::string& label)
{
    for (auto it = tags_.begin(); it != tags_.end(); ++it)
    {
        if (it->label() == label)
        {
            tags_.erase(it);
            return;
        }
    }
    throw PathNotFoundException("Tag not found: " + label);
}

bool FileMetadata::hasTag(const std::string& label) const
{
    return std::any_of(tags_.begin(), tags_.end(),
                       [&label](const Tag& tag) { return tag.label() == label; });
}

std::size_t FileMetadata::tagCount() const
{
    return tags_.size();
}

const std::string& FileMetadata::author() const
{
    return author_;
}

void FileMetadata::setAuthor(std::string author)
{
    author_ = std::move(author);
}

const std::string& FileMetadata::language() const
{
    return language_;
}

void FileMetadata::setLanguage(std::string language)
{
    language_ = std::move(language);
}

const std::string& FileMetadata::createdBy() const
{
    return createdBy_;
}

void FileMetadata::setCreatedBy(std::string login)
{
    createdBy_ = std::move(login);
}

void FileMetadata::toggleFavorite()
{
    isFavorite_ = !isFavorite_;
}

bool FileMetadata::isFavorite() const
{
    return isFavorite_;
}

void FileMetadata::setRating(int rating)
{
    if (rating < 0 || rating > 5)
    {
        throw FileSystemException("Rating must be 0..5");
    }
    rating_ = rating;
}

int FileMetadata::rating() const
{
    return rating_;
}

}
