#include "storage/BackupService.h"

#include "entry/File.h"
#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"

namespace fs
{

BackupRecord::BackupRecord(Path path, std::string content)
    : path_(std::move(path)), content_(std::move(content))
{
}

const Path& BackupRecord::path() const
{
    return path_;
}

const std::string& BackupRecord::content() const
{
    return content_;
}

void BackupService::capture(const Storage& storage)
{
    records_.clear();
    storage.walk([this](const Path& path, const FileSystemEntry& entry) {
        const File* file = dynamic_cast<const File*>(&entry);
        if (file != nullptr)
        {
            records_.emplace_back(path, file->read());
        }
    });
}

std::size_t BackupService::recordCount() const
{
    return records_.size();
}

bool BackupService::has(const Path& path) const
{
    for (const BackupRecord& record : records_)
    {
        if (record.path() == path)
        {
            return true;
        }
    }
    return false;
}

std::string BackupService::contentOf(const Path& path) const
{
    for (const BackupRecord& record : records_)
    {
        if (record.path() == path)
        {
            return record.content();
        }
    }
    throw PathNotFoundException("Not in backup: " + path.toString());
}

std::size_t BackupService::restoreTo(Storage& storage, Timestamp at) const
{
    std::size_t restored = 0;
    for (const BackupRecord& record : records_)
    {
        try
        {
            FileSystemEntry& entry = storage.resolve(record.path());
            File* file = dynamic_cast<File*>(&entry);
            if (file != nullptr)
            {
                file->write(record.content(), at);
                ++restored;
            }
        }
        catch (const PathNotFoundException&)
        {
            // путь исчез после снимка — пропускаем
        }
        catch (const PermissionDeniedException&)
        {
            // файл без права записи — пропускаем
        }
    }
    return restored;
}

}
