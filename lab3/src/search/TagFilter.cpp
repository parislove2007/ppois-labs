#include "search/TagFilter.h"

namespace fs
{

TagFilter::TagFilter(const Storage& storage, std::string label)
    : storage_(storage), label_(std::move(label))
{
}

bool TagFilter::matches(const Path& path, const FileSystemEntry&) const
{
    const FileMetadata* metadata = storage_.findMetadata(path);
    return metadata != nullptr && metadata->hasTag(label_);
}

}
