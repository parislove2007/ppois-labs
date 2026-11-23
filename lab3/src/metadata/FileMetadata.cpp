#include "metadata/FileMetadata.h"

#include "error/EntryAlreadyExistsException.h"
#include "error/PathNotFoundException.h"

namespace fs
{

FileMetadata::FileMetadata()
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
    for (const Tag& tag : tags_)
    {
        if (tag.label() == label)
        {
            return true;
        }
    }
    return false;
}

std::size_t FileMetadata::tagCount() const
{
    return tags_.size();
}

}
