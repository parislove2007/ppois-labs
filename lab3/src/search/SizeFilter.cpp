#include "search/SizeFilter.h"

namespace fs
{

SizeFilter::SizeFilter(FileSize min, FileSize max) : min_(min), max_(max)
{
}

bool SizeFilter::matches(const Path&, const FileSystemEntry& entry) const
{
    const FileSize size = entry.size();
    return min_.fitsIn(size) && size.fitsIn(max_);
}

}
