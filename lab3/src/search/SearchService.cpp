#include "search/SearchService.h"

namespace fs
{

std::vector<Path> SearchService::search(const Storage& storage,
                                        const SearchFilter& filter) const
{
    std::vector<Path> result;
    storage.walk([&result, &filter](const Path& path, const FileSystemEntry& entry) {
        if (filter.matches(path, entry))
        {
            result.push_back(path);
        }
    });
    return result;
}

}
