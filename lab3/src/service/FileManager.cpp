#include "service/FileManager.h"

#include <cstdint>

#include "entry/BinaryFile.h"
#include "entry/File.h"
#include "entry/TextFile.h"
#include "error/NotAFileException.h"
#include "error/PermissionDeniedException.h"
#include "operations/CopyOperation.h"
#include "operations/CreateOperation.h"
#include "operations/DeleteOperation.h"
#include "operations/MoveOperation.h"
#include "operations/RenameOperation.h"
#include "search/SearchService.h"
#include "value/FileExtension.h"

namespace fs
{

FileManager::FileManager(FileSize defaultQuota) : quotas_(defaultQuota)
{
}

const Storage& FileManager::storage() const
{
    return storage_;
}

Storage& FileManager::storage()
{
    return storage_;
}

const QuotaManager& FileManager::quotas() const
{
    return quotas_;
}

const TrashBin& FileManager::trash() const
{
    return trash_;
}

const OperationJournal& FileManager::journal() const
{
    return journal_;
}

void FileManager::createDirectory(const Path& path, const User& owner, Timestamp at)
{
    storage_.ensureDirectory(path, owner.login(), at);
    journal_.log(std::make_unique<CreateOperation>(path, at));
}

void FileManager::createFile(const Path& path, const std::string& content, const User& owner,
                             Timestamp at)
{
    quotas_.accountWritten(owner.login(),
                           FileSize::fromBytes(static_cast<std::int64_t>(content.size())));
    const Permission filePermission = owner.defaultPermission().withWrite();
    std::unique_ptr<FileSystemEntry> file;
    if (FileExtension::fromFileName(path.name()).isText())
    {
        file = std::make_unique<TextFile>(path.name(), owner.login(), filePermission, at);
    }
    else
    {
        file = std::make_unique<BinaryFile>(path.name(), owner.login(), filePermission, at);
    }
    dynamic_cast<File&>(*file).write(content, at);
    storage_.attach(path, std::move(file), at);
    journal_.log(std::make_unique<CreateOperation>(path, at));
}

void FileManager::write(const Path& path, const std::string& content, Timestamp at)
{
    FileSystemEntry& entry = storage_.resolve(path);
    File* file = dynamic_cast<File*>(&entry);
    if (file == nullptr)
    {
        throw NotAFileException("Not a file: " + path.toString());
    }
    if (!entry.permission().allows(Permission::readWrite()))
    {
        throw PermissionDeniedException("Write denied: " + path.toString());
    }
    chargeQuotaDelta(entry.owner(), file->read().size(), content.size());
    file->write(content, at);
}

void FileManager::saveVersion(const Path& path, const std::string& content, Timestamp at)
{
    write(path, content, at);
    versionHistoryFor(path).addVersion(content, at);
}

VersionHistory& FileManager::versionHistoryFor(const Path& path)
{
    return histories_[path.toString()];
}

void FileManager::rollback(const Path& path, int versionNumber, Timestamp at)
{
    VersionHistory& history = versionHistoryFor(path);
    const EntryVersion version = history.rollbackTo(versionNumber, at);
    write(path, version.content(), at);
}

void FileManager::remove(const Path& path, Timestamp at)
{
    trash_.moveToTrash(path, storage_.detach(path, at));
    journal_.log(std::make_unique<DeleteOperation>(path, at));
}

void FileManager::restoreFromTrash(std::size_t index, Timestamp at)
{
    auto restored = trash_.restore(index);
    storage_.attach(restored.first, std::move(restored.second), at);
    journal_.log(std::make_unique<CreateOperation>(restored.first, at));
}

void FileManager::move(const Path& from, const Path& to, Timestamp at)
{
    std::unique_ptr<FileSystemEntry> entry = storage_.detach(from, at);
    entry->rename(to.name(), at);
    storage_.attach(to, std::move(entry), at);
    journal_.log(std::make_unique<MoveOperation>(from, to, at));
}

void FileManager::copyFile(const Path& from, const Path& to, Timestamp at)
{
    FileSystemEntry& entry = storage_.resolve(from);
    File* file = dynamic_cast<File*>(&entry);
    if (file == nullptr)
    {
        throw NotAFileException("Only files can be copied: " + from.toString());
    }
    quotas_.accountWritten(entry.owner(),
                           FileSize::fromBytes(static_cast<std::int64_t>(file->read().size())));
    const Permission copyPermission = entry.permission().withWrite();
    std::unique_ptr<FileSystemEntry> copy;
    if (FileExtension::fromFileName(to.name()).isText())
    {
        copy = std::make_unique<TextFile>(to.name(), entry.owner(), copyPermission, at);
    }
    else
    {
        copy = std::make_unique<BinaryFile>(to.name(), entry.owner(), copyPermission, at);
    }
    dynamic_cast<File&>(*copy).write(file->read(), at);
    storage_.attach(to, std::move(copy), at);
    journal_.log(std::make_unique<CopyOperation>(from, to, at));
}

void FileManager::rename(const Path& path, const std::string& newName, Timestamp at)
{
    storage_.resolve(path).rename(newName, at);
    journal_.log(std::make_unique<RenameOperation>(path, newName, at));
}

void FileManager::tag(const Path& path, Tag tag)
{
    storage_.metadataFor(path).addTag(std::move(tag));
}

void FileManager::describe(const Path& path, std::string description)
{
    storage_.metadataFor(path).setDescription(std::move(description));
}

std::vector<Path> FileManager::search(const SearchFilter& filter) const
{
    return SearchService().search(storage_, filter);
}

void FileManager::makeBackup()
{
    backup_.capture(storage_);
}

std::size_t FileManager::restoreBackup(Timestamp at)
{
    return backup_.restoreTo(storage_, at);
}

void FileManager::chargeQuotaDelta(const std::string& login, std::uint64_t oldBytes,
                                   std::uint64_t newBytes)
{
    if (newBytes >= oldBytes)
    {
        quotas_.accountWritten(
            login, FileSize::fromBytes(static_cast<std::int64_t>(newBytes - oldBytes)));
    }
    else
    {
        quotas_.accountFreed(
            login, FileSize::fromBytes(static_cast<std::int64_t>(oldBytes - newBytes)));
    }
}

}
