#include "search/DateFilter.h"

namespace fs
{

DateFilter::DateFilter(Timestamp from, Timestamp to) : from_(from), to_(to)
{
}

bool DateFilter::matches(const Path&, const FileSystemEntry& entry) const
{
    const Timestamp modified = entry.modified();
    return !modified.isBefore(from_) && !modified.isAfter(to_);
}

}
