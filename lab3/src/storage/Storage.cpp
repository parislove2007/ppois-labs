#include "storage/Storage.h"

#include "error/EntryAlreadyExistsException.h"
#include "error/FileSystemException.h"
#include "error/NotADirectoryException.h"
#include "error/PathNotFoundException.h"
#include "error/StorageFullException.h"

namespace fs
{

Storage::Storage() : Storage(FileSize())
{
}

Storage::Storage(FileSize capacity)
    : root_("/", "system", Permission::readWrite(), Timestamp()), capacity_(capacity)
{
}

Directory& Storage::root()
{
    return root_;
}

const Directory& Storage::root() const
{
    return root_;
}

FileSystemEntry& Storage::resolve(const Path& path)
{
    FileSystemEntry* current = &root_;
    for (const std::string& segment : path.segments())
    {
        if (!current->isDirectory())
        {
            throw NotADirectoryException("Not a directory: " + current->name());
        }
        current = &dynamic_cast<Directory&>(*current).find(segment);
    }
    return *current;
}

const FileSystemEntry& Storage::resolve(const Path& path) const
{
    return const_cast<Storage*>(this)->resolve(path);
}

Directory& Storage::ensureDirectory(const Path& path, const std::string& owner, Timestamp at)
{
    Directory* current = &root_;
    std::string built;
    for (const std::string& segment : path.segments())
    {
        built += "/" + segment;
        if (current->contains(segment))
        {
            FileSystemEntry& found = current->find(segment);
            if (!found.isDirectory())
            {
                throw EntryAlreadyExistsException("File blocks path: " + built);
            }
            current = &dynamic_cast<Directory&>(found);
        }
        else
        {
            auto created = std::make_unique<Directory>(segment, owner, Permission::readWrite(),
                                                       at);
            Directory* raw = created.get();
            current->add(std::move(created), at);
            current = raw;
        }
    }
    return *current;
}

void Storage::attach(const Path& path, std::unique_ptr<FileSystemEntry> entry, Timestamp at)
{
    ensureCapacity(*entry);
    Directory& parent = parentDirectoryFor(path);
    parent.add(std::move(entry), at);
}

void Storage::ensureCapacity(const FileSystemEntry& entry) const
{
    if (capacity_.isZero())
    {
        return;
    }
    if (!root_.size().fitsIn(capacity_ - entry.size()) && !entry.size().isZero())
    {
        throw StorageFullException("Storage capacity exceeded");
    }
}

std::unique_ptr<FileSystemEntry> Storage::detach(const Path& path, Timestamp at)
{
    if (path.toString() == "/")
    {
        throw FileSystemException("Root cannot be detached");
    }
    Directory& parent = parentDirectoryFor(path);
    return parent.remove(path.name(), at);
}

FileMetadata& Storage::metadataFor(const Path& path)
{
    return metadata_[path.toString()];
}

bool Storage::hasMetadata(const Path& path) const
{
    return metadata_.count(path.toString()) > 0;
}

const FileMetadata* Storage::findMetadata(const Path& path) const
{
    auto it = metadata_.find(path.toString());
    return it == metadata_.end() ? nullptr : &it->second;
}

void Storage::walk(
    const std::function<void(const Path&, const FileSystemEntry&)>& visitor) const
{
    walkFrom(Path::parse("/"), root_, visitor);
}

Directory& Storage::parentDirectoryFor(const Path& path)
{
    FileSystemEntry& parent = resolve(path.parent());
    if (!parent.isDirectory())
    {
        throw NotADirectoryException("Parent is not a directory: " + path.parent().toString());
    }
    return dynamic_cast<Directory&>(parent);
}

void Storage::walkFrom(
    const Path& path, const Directory& directory,
    const std::function<void(const Path&, const FileSystemEntry&)>& visitor)
{
    visitor(path, directory);
    for (const std::unique_ptr<FileSystemEntry>& entry : directory.entries())
    {
        const Path childPath = path.join(Path::parse(entry->name()));
        if (entry->isDirectory())
        {
            walkFrom(childPath, dynamic_cast<const Directory&>(*entry), visitor);
        }
        else
        {
            visitor(childPath, *entry);
        }
    }
}

}
