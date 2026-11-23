#include "search/NameFilter.h"

namespace fs
{

NameFilter::NameFilter(std::string needle) : needle_(std::move(needle))
{
}

bool NameFilter::matches(const Path&, const FileSystemEntry& entry) const
{
    return entry.name().find(needle_) != std::string::npos;
}

}
