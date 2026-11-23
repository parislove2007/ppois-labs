#include "search/CompositeFilter.h"

namespace fs
{

CompositeFilter::CompositeFilter(bool matchAll) : matchAll_(matchAll)
{
}

void CompositeFilter::add(std::unique_ptr<SearchFilter> filter)
{
    filters_.push_back(std::move(filter));
}

bool CompositeFilter::matches(const Path& path, const FileSystemEntry& entry) const
{
    for (const std::unique_ptr<SearchFilter>& filter : filters_)
    {
        const bool matched = filter->matches(path, entry);
        if (matchAll_ && !matched)
        {
            return false;
        }
        if (!matchAll_ && matched)
        {
            return true;
        }
    }
    return matchAll_;
}

}
