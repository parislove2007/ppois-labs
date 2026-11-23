#include "service/DirectoryListing.h"

namespace fs
{

DirectoryListing::DirectoryListing(Path path, std::vector<std::string> entryNames,
                                   std::size_t fileCount, std::size_t directoryCount,
                                   FileSize totalBytes)
    : path_(std::move(path)),
      entryNames_(std::move(entryNames)),
      fileCount_(fileCount),
      directoryCount_(directoryCount),
      totalBytes_(totalBytes)
{
}

const Path& DirectoryListing::path() const
{
    return path_;
}

const std::vector<std::string>& DirectoryListing::entryNames() const
{
    return entryNames_;
}

std::size_t DirectoryListing::fileCount() const
{
    return fileCount_;
}

std::size_t DirectoryListing::directoryCount() const
{
    return directoryCount_;
}

FileSize DirectoryListing::totalBytes() const
{
    return totalBytes_;
}

std::string DirectoryListing::toLine() const
{
    std::string line = path_.toString() + ":";
    for (const std::string& name : entryNames_)
    {
        line += " " + name;
    }
    return line;
}

}
