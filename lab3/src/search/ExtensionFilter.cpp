#include "search/ExtensionFilter.h"

namespace fs
{

ExtensionFilter::ExtensionFilter(FileExtension extension) : extension_(std::move(extension))
{
}

bool ExtensionFilter::matches(const Path&, const FileSystemEntry& entry) const
{
    return FileExtension::fromFileName(entry.name()) == extension_;
}

}
