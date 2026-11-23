#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "entry/SymbolicLink.h"
#include "entry/TextFile.h"
#include "error/EmptyTrashException.h"
#include "error/EntryAlreadyExistsException.h"
#include "error/FileSystemException.h"
#include "error/NotADirectoryException.h"
#include "error/PathNotFoundException.h"
#include "error/QuotaExceededException.h"
#include "storage/BackupService.h"
#include "storage/QuotaManager.h"
#include "storage/Storage.h"
#include "storage/TrashBin.h"
#include "value/Path.h"
#include "value/Permission.h"
#include "value/Timestamp.h"

using namespace fs;

namespace
{

std::unique_ptr<TextFile> makeFile(const std::string& name, const std::string& content)
{
    auto file = std::make_unique<TextFile>(name, "ivan", Permission::readWrite(),
                                           Timestamp::of(1));
    file->write(content, Timestamp::of(2));
    return file;
}

Storage makeSampleStorage()
{
    Storage storage;
    storage.ensureDirectory(Path::parse("/docs"), "ivan", Timestamp::of(1));
    storage.attach(Path::parse("/docs/notes.txt"), makeFile("notes.txt", "hello"),
                   Timestamp::of(2));
    storage.attach(Path::parse("/link"),
                   std::make_unique<SymbolicLink>("link", Path::parse("/docs/notes.txt"),
                                                  "ivan", Permission::readOnly(),
                                                  Timestamp::of(3)),
                   Timestamp::of(3));
    return storage;
}

} // namespace

TEST(StorageTest, ResolvesEntriesByPath)
{
    Storage storage = makeSampleStorage();
    EXPECT_EQ(storage.root().name(), "/");
    EXPECT_EQ(storage.resolve(Path::parse("/")).name(), "/");
    EXPECT_EQ(storage.resolve(Path::parse("/docs/notes.txt")).name(), "notes.txt");
    EXPECT_THROW(storage.resolve(Path::parse("/docs/missing.txt")), PathNotFoundException);
}

TEST(StorageTest, ResolveThroughFileFails)
{
    Storage storage = makeSampleStorage();
    EXPECT_THROW(storage.resolve(Path::parse("/docs/notes.txt/inner")),
                 NotADirectoryException);
}

TEST(StorageTest, EnsureDirectoryCreatesChain)
{
    Storage storage = makeSampleStorage();
    Directory& deep = storage.ensureDirectory(Path::parse("/docs/a/b"), "ivan",
                                              Timestamp::of(5));
    EXPECT_EQ(deep.name(), "b");
    EXPECT_NO_THROW(storage.resolve(Path::parse("/docs/a/b")));
    EXPECT_THROW(storage.ensureDirectory(Path::parse("/docs/notes.txt/x"), "ivan",
                                         Timestamp::of(6)),
                 EntryAlreadyExistsException);
}

TEST(StorageTest, AttachAndDetach)
{
    Storage storage = makeSampleStorage();
    storage.attach(Path::parse("/docs/extra.txt"), makeFile("extra.txt", "x"),
                   Timestamp::of(4));
    EXPECT_EQ(dynamic_cast<TextFile&>(storage.resolve(Path::parse("/docs/extra.txt"))).read(),
              "x");

    auto detached = storage.detach(Path::parse("/docs/extra.txt"), Timestamp::of(5));
    EXPECT_EQ(detached->name(), "extra.txt");
    EXPECT_THROW(storage.resolve(Path::parse("/docs/extra.txt")), PathNotFoundException);
    EXPECT_THROW(storage.detach(Path::parse("/"), Timestamp::of(1)), FileSystemException);
}

TEST(StorageTest, MetadataIsLazilyCreated)
{
    Storage storage = makeSampleStorage();
    const Path path = Path::parse("/docs/notes.txt");
    EXPECT_FALSE(storage.hasMetadata(path));
    EXPECT_EQ(storage.findMetadata(path), nullptr);
    storage.metadataFor(path).addTag(Tag("work"));
    EXPECT_TRUE(storage.hasMetadata(path));
    EXPECT_NE(storage.findMetadata(path), nullptr);
    EXPECT_TRUE(storage.findMetadata(path)->hasTag("work"));
}

TEST(StorageTest, WalkVisitsEveryEntry)
{
    Storage storage = makeSampleStorage();
    std::vector<std::string> visited;
    storage.walk([&visited](const Path& path, const FileSystemEntry&) {
        visited.push_back(path.toString());
    });
    EXPECT_EQ(visited.size(), 4u);
    EXPECT_EQ(visited[0], "/");
    EXPECT_EQ(visited[1], "/docs");
    EXPECT_EQ(visited[2], "/docs/notes.txt");
    EXPECT_EQ(visited[3], "/link");
}

TEST(QuotaManagerTest, EnforcesLimits)
{
    QuotaManager quotas(FileSize::fromKilobytes(1));
    quotas.setQuota("anna", FileSize::fromBytes(100));
    EXPECT_TRUE(quotas.quotaOf("anna").fitsIn(FileSize::fromBytes(100)));
    EXPECT_TRUE(quotas.quotaOf("ivan").fitsIn(FileSize::fromKilobytes(1)));

    quotas.accountWritten("anna", FileSize::fromBytes(70));
    EXPECT_EQ(quotas.usedOf("anna").bytes(), 70u);
    EXPECT_THROW(quotas.accountWritten("anna", FileSize::fromBytes(31)),
                 QuotaExceededException);

    quotas.accountFreed("anna", FileSize::fromBytes(30));
    EXPECT_EQ(quotas.usedOf("anna").bytes(), 40u);
    quotas.accountFreed("anna", FileSize::fromBytes(1000));
    EXPECT_EQ(quotas.usedOf("anna").bytes(), 0u);
}

TEST(TrashBinTest, KeepsOriginAndRestores)
{
    TrashBin trash;
    EXPECT_THROW(trash.entryAt(0), EmptyTrashException);

    trash.moveToTrash(Path::parse("/docs/old.txt"), makeFile("old.txt", "old"));
    EXPECT_EQ(trash.itemCount(), 1u);
    EXPECT_EQ(trash.entryAt(0).name(), "old.txt");
    EXPECT_EQ(trash.originAt(0).toString(), "/docs/old.txt");

    auto restored = trash.restore(0);
    EXPECT_EQ(restored.first.toString(), "/docs/old.txt");
    EXPECT_EQ(restored.second->name(), "old.txt");
    EXPECT_EQ(trash.itemCount(), 0u);
    EXPECT_THROW(trash.restore(5), EmptyTrashException);

    trash.moveToTrash(Path::parse("/a"), makeFile("a", "a"));
    EXPECT_THROW(trash.entryAt(3), PathNotFoundException);
    trash.clear();
    EXPECT_EQ(trash.itemCount(), 0u);
}

TEST(BackupServiceTest, CapturesFilesOnly)
{
    Storage storage = makeSampleStorage();
    BackupService backup;
    backup.capture(storage);
    EXPECT_EQ(backup.recordCount(), 1u);
    EXPECT_TRUE(backup.has(Path::parse("/docs/notes.txt")));
    EXPECT_FALSE(backup.has(Path::parse("/link")));
    EXPECT_EQ(backup.contentOf(Path::parse("/docs/notes.txt")), "hello");
    EXPECT_THROW(backup.contentOf(Path::parse("/nope")), PathNotFoundException);
}

TEST(BackupServiceTest, RestoresOverwrittenContent)
{
    Storage storage = makeSampleStorage();
    BackupService backup;
    backup.capture(storage);

    dynamic_cast<TextFile&>(storage.resolve(Path::parse("/docs/notes.txt")))
        .write("changed", Timestamp::of(9));
    EXPECT_EQ(
        dynamic_cast<TextFile&>(storage.resolve(Path::parse("/docs/notes.txt"))).read(),
        "changed");

    const std::size_t restored = backup.restoreTo(storage, Timestamp::of(10));
    EXPECT_EQ(restored, 1u);
    FileSystemEntry& entry = storage.resolve(Path::parse("/docs/notes.txt"));
    EXPECT_EQ(dynamic_cast<TextFile&>(entry).read(), "hello");
}
