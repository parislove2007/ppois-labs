#include "storage/BackupService.h"

#include <algorithm>

#include "entry/File.h"
#include "value/Checksum.h"
#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"

namespace fs
{

BackupRecord::BackupRecord(Path path, std::string content)
    : path_(std::move(path)),
      content_(std::move(content)),
      checksum_(Checksum::calculate(content_))
{
}

const Checksum& BackupRecord::checksum() const
{
    return checksum_;
}

bool BackupRecord::verify(const std::string& content) const
{
    return checksum_.verify(content);
}

const Path& BackupRecord::path() const
{
    return path_;
}

const std::string& BackupRecord::content() const
{
    return content_;
}

BackupService::BackupService() : isEncrypted_(false)
{
}

void BackupService::capture(const Storage& storage)
{
    capture(storage, "", Timestamp());
}

void BackupService::capture(const Storage& storage, std::string label, Timestamp createdAt)
{
    records_.clear();
    label_ = std::move(label);
    createdAt_ = createdAt;
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

const std::string& BackupService::label() const
{
    return label_;
}

Timestamp BackupService::createdAt() const
{
    return createdAt_;
}

void BackupService::toggleEncryption()
{
    isEncrypted_ = !isEncrypted_;
}

bool BackupService::isEncrypted() const
{
    return isEncrypted_;
}

std::string BackupService::describe() const
{
    return "backup '" + label_ + "' at " + std::to_string(createdAt_.seconds()) + "s with "
         + std::to_string(records_.size()) + " records"
         + (isEncrypted_ ? " (encrypted)" : "");
}

bool BackupService::has(const Path& path) const
{
    return std::any_of(records_.begin(), records_.end(),
                       [&path](const BackupRecord& record) { return record.path() == path; });
}

std::string BackupService::contentOf(const Path& path) const
{
    const auto it = std::find_if(records_.begin(), records_.end(),
                                 [&path](const BackupRecord& record) {
                                     return record.path() == path;
                                 });
    if (it == records_.end())
    {
        throw PathNotFoundException("Not in backup: " + path.toString());
    }
    return it->content();
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
