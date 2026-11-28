#include "search/NameFilter.h"

#include <algorithm>
#include <cctype>

namespace fs
{

namespace
{

std::string lowered(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return text;
}

} // namespace

NameFilter::NameFilter(std::string needle) : NameFilter(std::move(needle), true)
{
}

NameFilter::NameFilter(std::string needle, bool caseSensitive)
    : needle_(std::move(needle)), caseSensitive_(caseSensitive)
{
}

bool NameFilter::matches(const Path&, const FileSystemEntry& entry) const
{
    if (caseSensitive_)
    {
        return entry.name().find(needle_) != std::string::npos;
    }
    return lowered(entry.name()).find(lowered(needle_)) != std::string::npos;
}

}
