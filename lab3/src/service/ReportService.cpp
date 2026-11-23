#include "service/ReportService.h"

#include <algorithm>
#include <memory>
#include <string>

#include "entry/SymbolicLink.h"

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

std::string ReportService::summaryLine(const Storage& storage) const
{
    std::size_t directories = 0;
    std::size_t files = 0;
    std::size_t links = 0;
    storage.walk([&directories, &files, &links](const Path&, const FileSystemEntry& entry) {
        if (entry.isDirectory())
        {
            ++directories;
        }
        else if (dynamic_cast<const SymbolicLink*>(&entry) != nullptr)
        {
            ++links;
        }
        else
        {
            ++files;
        }
    });
    return std::to_string(directories) + " directories, " + std::to_string(files)
         + " files, " + std::to_string(links) + " links, total "
         + std::to_string(storage.root().size().bytes()) + " bytes";
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
