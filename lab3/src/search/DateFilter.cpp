#include "search/DateFilter.h"

namespace fs
{

DateFilter::DateFilter(Timestamp from, Timestamp to) : DateFilter(from, to, false)
{
}

DateFilter::DateFilter(Timestamp from, Timestamp to, bool useCreated)
    : from_(from), to_(to), useCreated_(useCreated)
{
}

bool DateFilter::matches(const Path&, const FileSystemEntry& entry) const
{
    const Timestamp moment = useCreated_ ? entry.created() : entry.modified();
    return !moment.isBefore(from_) && !moment.isAfter(to_);
}

}
