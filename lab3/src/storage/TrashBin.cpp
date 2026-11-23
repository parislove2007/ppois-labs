#include "storage/TrashBin.h"

#include "error/EmptyTrashException.h"
#include "error/PathNotFoundException.h"

namespace fs
{

void TrashBin::moveToTrash(Path origin, std::unique_ptr<FileSystemEntry> entry)
{
    items_.push_back(Item{std::move(origin), std::move(entry)});
}

std::size_t TrashBin::itemCount() const
{
    return items_.size();
}

const FileSystemEntry& TrashBin::entryAt(std::size_t index) const
{
    checkIndex(index);
    return *items_[index].entry;
}

Path TrashBin::originAt(std::size_t index) const
{
    checkIndex(index);
    return items_[index].origin;
}

std::pair<Path, std::unique_ptr<FileSystemEntry>> TrashBin::restore(std::size_t index)
{
    checkIndex(index);
    std::pair<Path, std::unique_ptr<FileSystemEntry>> result{items_[index].origin,
                                                             std::move(items_[index].entry)};
    items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(index));
    return result;
}

void TrashBin::clear()
{
    items_.clear();
}

void TrashBin::checkIndex(std::size_t index) const
{
    if (items_.empty())
    {
        throw EmptyTrashException("Trash bin is empty");
    }
    if (index >= items_.size())
    {
        throw PathNotFoundException("No trashed item #" + std::to_string(index));
    }
}

}
