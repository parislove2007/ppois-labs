#include "entry/Directory.h"

#include "error/EntryAlreadyExistsException.h"
#include "error/PathNotFoundException.h"

namespace fs
{

void Directory::add(std::unique_ptr<FileSystemEntry> entry, Timestamp at)
{
    if (contains(entry->name()))
    {
        throw EntryAlreadyExistsException("Name already exists: " + entry->name());
    }
    entries_.push_back(std::move(entry));
    touch(at);
}

std::unique_ptr<FileSystemEntry> Directory::remove(const std::string& name, Timestamp at)
{
    auto it = findIterator(name);
    if (it == entries_.end())
    {
        throw PathNotFoundException("Entry not found: " + name);
    }
    std::unique_ptr<FileSystemEntry> result = std::move(*it);
    entries_.erase(it);
    touch(at);
    return result;
}

FileSystemEntry& Directory::find(const std::string& name)
{
    auto it = findIterator(name);
    if (it == entries_.end())
    {
        throw PathNotFoundException("Entry not found: " + name);
    }
    return **it;
}

bool Directory::contains(const std::string& name) const
{
    for (const std::unique_ptr<FileSystemEntry>& entry : entries_)
    {
        if (entry->name() == name)
        {
            return true;
        }
    }
    return false;
}

std::size_t Directory::entryCount() const
{
    return entries_.size();
}

const std::vector<std::unique_ptr<FileSystemEntry>>& Directory::entries() const
{
    return entries_;
}

FileSize Directory::size() const
{
    FileSize total;
    for (const std::unique_ptr<FileSystemEntry>& entry : entries_)
    {
        total = total + entry->size();
    }
    return total;
}

std::string Directory::describe() const
{
    return "[dir] " + name_ + " (" + std::to_string(entries_.size()) + " entries)";
}

bool Directory::isDirectory() const
{
    return true;
}

std::vector<std::unique_ptr<FileSystemEntry>>::iterator Directory::findIterator(
    const std::string& name)
{
    for (auto it = entries_.begin(); it != entries_.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            return it;
        }
    }
    return entries_.end();
}

}
