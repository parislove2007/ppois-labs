#include "service/ReportService.h"

#include <algorithm>
#include <memory>
#include <string>

#include "entry/SymbolicLink.h"
#include "error/NotADirectoryException.h"

namespace fs
{

namespace
{

void renderDirectory(const Directory& directory, int depth, std::string& text)
{
    for (const std::unique_ptr<FileSystemEntry>& entry : directory.entries())
    {
        text.append(static_cast<std::size_t>(depth) * 2, ' ');
        text += entry->name() + "\n";
        if (entry->isDirectory())
        {
            renderDirectory(dynamic_cast<const Directory&>(*entry), depth + 1, text);
        }
    }
}

} // namespace

std::string ReportService::treeText(const Storage& storage) const
{
    std::string text = "/\n";
    renderDirectory(storage.root(), 1, text);
    return text;
}

StorageStats ReportService::computeStats(const Storage& storage) const
{
    StorageStats stats;
    storage.walk([&stats](const Path&, const FileSystemEntry& entry) {
        if (entry.isDirectory())
        {
            stats.countDirectory();
        }
        else if (dynamic_cast<const SymbolicLink*>(&entry) != nullptr)
        {
            stats.countLink();
        }
        else
        {
            stats.countFile();
        }
        stats.addBytes(entry.size());
    });
    stats.stamp(storage.root().modified());
    return stats;
}

std::string ReportService::summaryLine(const Storage& storage) const
{
    return computeStats(storage).toLine();
}

std::vector<FileInfo> ReportService::collectInfo(const Storage& storage) const
{
    std::vector<FileInfo> result;
    storage.walk([&result](const Path& path, const FileSystemEntry& entry) {
        std::string kind = "file";
        if (entry.isDirectory())
        {
            kind = "dir";
        }
        else if (dynamic_cast<const SymbolicLink*>(&entry) != nullptr)
        {
            kind = "link";
        }
        result.emplace_back(path, entry.owner(), kind, entry.size(), entry.isDirectory(),
                            entry.modified(), entry.permission());
    });
    return result;
}

DirectoryListing ReportService::listDirectory(const Storage& storage,
                                              const Path& path) const
{
    const FileSystemEntry& entry = storage.resolve(path);
    if (!entry.isDirectory())
    {
        throw NotADirectoryException("Not a directory: " + path.toString());
    }
    const Directory& directory = dynamic_cast<const Directory&>(entry);
    std::vector<std::string> names;
    std::size_t files = 0;
    std::size_t directories = 0;
    for (const std::unique_ptr<FileSystemEntry>& child : directory.entries())
    {
        names.push_back(child->name());
        if (child->isDirectory())
        {
            ++directories;
        }
        else
        {
            ++files;
        }
    }
    return DirectoryListing(path, std::move(names), files, directories, directory.size());
}

std::vector<std::pair<Path, FileSize>> ReportService::topLargest(const Storage& storage,
                                                                 std::size_t count) const
{
    std::vector<std::pair<Path, FileSize>> entries;
    storage.walk([&entries](const Path& path, const FileSystemEntry& entry) {
        if (!entry.isDirectory())
        {
            entries.emplace_back(path, entry.size());
        }
    });
    std::sort(entries.begin(), entries.end(),
              [](const std::pair<Path, FileSize>& left, const std::pair<Path, FileSize>& right) {
                  return right.second < left.second;
              });
    if (entries.size() > count)
    {
        entries.resize(count);
    }
    return entries;
}

}
