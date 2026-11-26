#include "service/SearchQuery.h"

#include <limits>

#include "error/FileSystemException.h"
#include "search/CompositeFilter.h"
#include "search/DateFilter.h"
#include "search/ExtensionFilter.h"
#include "search/NameFilter.h"
#include "search/SizeFilter.h"
#include "search/TagFilter.h"
#include "storage/Storage.h"
#include "value/Timestamp.h"

namespace fs
{

SearchQuery::SearchQuery()
    : caseSensitive_(true),
      minSize_(FileSize()),
      maxSize_(FileSize()),
      modifiedAfter_(Timestamp()),
      hasName_(false),
      hasExtension_(false),
      hasSize_(false),
      hasTime_(false),
      hasTag_(false)
{
}

SearchQuery& SearchQuery::withName(std::string needle, bool caseSensitive)
{
    needle_ = std::move(needle);
    caseSensitive_ = caseSensitive;
    hasName_ = true;
    return *this;
}

SearchQuery& SearchQuery::withExtension(FileExtension extension)
{
    extension_ = std::move(extension);
    hasExtension_ = true;
    return *this;
}

SearchQuery& SearchQuery::withSizeRange(FileSize min, FileSize max)
{
    minSize_ = min;
    maxSize_ = max;
    hasSize_ = true;
    return *this;
}

SearchQuery& SearchQuery::withModifiedAfter(Timestamp from)
{
    modifiedAfter_ = from;
    hasTime_ = true;
    return *this;
}

SearchQuery& SearchQuery::withTag(std::string label)
{
    tag_ = std::move(label);
    hasTag_ = true;
    return *this;
}

bool SearchQuery::isEmpty() const
{
    return !hasName_ && !hasExtension_ && !hasSize_ && !hasTime_ && !hasTag_;
}

std::unique_ptr<SearchFilter> SearchQuery::buildFilter(const Storage& storage) const
{
    if (isEmpty())
    {
        throw FileSystemException("Search query has no criteria");
    }
    auto combined = std::make_unique<CompositeFilter>(true);
    if (hasName_)
    {
        combined->add(std::make_unique<NameFilter>(needle_, caseSensitive_));
    }
    if (hasExtension_)
    {
        combined->add(std::make_unique<ExtensionFilter>(extension_));
    }
    if (hasSize_)
    {
        combined->add(std::make_unique<SizeFilter>(minSize_, maxSize_));
    }
    if (hasTime_)
    {
        combined->add(std::make_unique<DateFilter>(
            modifiedAfter_, Timestamp::of(std::numeric_limits<std::int64_t>::max())));
    }
    if (hasTag_)
    {
        combined->add(std::make_unique<TagFilter>(storage, tag_));
    }
    return combined;
}

}
