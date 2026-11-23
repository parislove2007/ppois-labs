#include "storage/TrashBin.h"

#include "error/EmptyTrashException.h"
#include "error/PathNotFoundException.h"
#include "error/StorageFullException.h"

namespace fs
{

TrashBin::TrashBin() : TrashBin(0)
{
}

TrashBin::TrashBin(std::size_t maxItems)
    : maxItems_(maxItems), restoredCount_(0), clearedCount_(0)
{
}

void TrashBin::moveToTrash(Path origin, std::unique_ptr<FileSystemEntry> entry)
{
    if (maxItems_ != 0 && items_.size() >= maxItems_)
    {
        throw StorageFullException("Trash bin is full");
    }
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
    ++restoredCount_;
    return result;
}

void TrashBin::clear()
{
    clearedCount_ += items_.size();
    items_.clear();
}

std::size_t TrashBin::restoredCount() const
{
    return restoredCount_;
}

std::size_t TrashBin::clearedCount() const
{
    return clearedCount_;
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
