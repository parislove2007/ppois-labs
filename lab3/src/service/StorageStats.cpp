#include "service/StorageStats.h"

#include <string>

namespace fs
{

StorageStats::StorageStats()
    : directories_(0), files_(0), links_(0)
{
}

void StorageStats::countDirectory()
{
    ++directories_;
}

void StorageStats::countFile()
{
    ++files_;
}

void StorageStats::countLink()
{
    ++links_;
}

void StorageStats::addBytes(FileSize size)
{
    totalBytes_ = totalBytes_ + size;
}

std::size_t StorageStats::directories() const
{
    return directories_;
}

std::size_t StorageStats::files() const
{
    return files_;
}

std::size_t StorageStats::links() const
{
    return links_;
}

FileSize StorageStats::totalBytes() const
{
    return totalBytes_;
}

void StorageStats::stamp(Timestamp at)
{
    computedAt_ = at;
}

Timestamp StorageStats::computedAt() const
{
    return computedAt_;
}

std::string StorageStats::toLine() const
{
    return std::to_string(directories_) + " directories, " + std::to_string(files_)
         + " files, " + std::to_string(links_) + " links, total "
         + std::to_string(totalBytes_.bytes()) + " bytes";
}

}
