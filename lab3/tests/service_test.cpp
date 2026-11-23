#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "entry/TextFile.h"
#include "error/NotAFileException.h"
#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"
#include "error/QuotaExceededException.h"
#include "metadata/Tag.h"
#include "operations/OperationJournal.h"
#include "search/NameFilter.h"
#include "service/FileManager.h"
#include "service/ReportService.h"
#include "user/Administrator.h"
#include "user/Guest.h"
#include "user/RegularUser.h"
#include "value/Path.h"
#include "value/Timestamp.h"

using namespace fs;

namespace
{

const Path kNotes = Path::parse("/docs/notes.txt");

std::string readOf(FileManager& manager, const Path& path)
{
    return dynamic_cast<const TextFile&>(manager.storage().resolve(path)).read();
}

} // namespace

TEST(FileManagerTest, CreatesFilesByExtension)
{
    FileManager manager(FileSize::fromKilobytes(64));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(1));
    manager.createFile(kNotes, "hello", ivan, Timestamp::of(2));
    manager.createFile(Path::parse("/docs/data.bin"), "AB", ivan, Timestamp::of(3));

    EXPECT_EQ(readOf(manager, kNotes), "hello");
    EXPECT_EQ(manager.storage().resolve(kNotes).describe(), "[text] notes.txt (5 bytes)");
    EXPECT_EQ(manager.storage().resolve(Path::parse("/docs/data.bin")).describe(),
              "[binary] data.bin (2 bytes)");
}

TEST(FileManagerTest, WriteEnforcesPermissionsAndQuota)
{
    FileManager manager(FileSize::fromBytes(10));
    Guest guest("guest", "Guest", Timestamp::of(0));
    manager.createFile(Path::parse("/small.txt"), "12345", guest, Timestamp::of(1));
    EXPECT_EQ(manager.quotas().usedOf("guest").bytes(), 5u);

    manager.write(Path::parse("/small.txt"), "12345678", Timestamp::of(2));
    EXPECT_EQ(manager.quotas().usedOf("guest").bytes(), 8u);
    EXPECT_THROW(manager.write(Path::parse("/small.txt"), "12345678901", Timestamp::of(3)),
                 QuotaExceededException);
    EXPECT_THROW(manager.write(Path::parse("/"), "x", Timestamp::of(4)), NotAFileException);
}

TEST(FileManagerTest, ReadOnlyEntryDeniesWrite)
{
    FileManager manager(FileSize::fromKilobytes(1));
    Administrator admin("root", "Root", Timestamp::of(0));
    manager.createFile(Path::parse("/system.log"), "init", admin, Timestamp::of(1));
    manager.storage().resolve(Path::parse("/system.log"))
        .changePermission(Permission::readOnly());
    EXPECT_THROW(manager.write(Path::parse("/system.log"), "hack", Timestamp::of(2)),
                 PermissionDeniedException);
}

TEST(FileManagerTest, RemoveMovesToTrashAndRestores)
{
    FileManager manager(FileSize::fromKilobytes(1));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(0));
    manager.createFile(kNotes, "data", ivan, Timestamp::of(1));

    manager.remove(kNotes, Timestamp::of(2));
    EXPECT_EQ(manager.trash().itemCount(), 1u);
    EXPECT_THROW(manager.storage().resolve(kNotes), PathNotFoundException);

    manager.restoreFromTrash(0, Timestamp::of(3));
    EXPECT_EQ(readOf(manager, kNotes), "data");
    EXPECT_EQ(manager.trash().itemCount(), 0u);
}

TEST(FileManagerTest, MoveCopyAndRename)
{
    FileManager manager(FileSize::fromKilobytes(1));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(0));
    manager.createFile(kNotes, "payload", ivan, Timestamp::of(1));

    manager.copyFile(kNotes, Path::parse("/docs/copy.txt"), Timestamp::of(2));
    EXPECT_EQ(readOf(manager, Path::parse("/docs/copy.txt")), "payload");

    manager.move(kNotes, Path::parse("/docs/moved.txt"), Timestamp::of(3));
    EXPECT_THROW(manager.storage().resolve(kNotes), PathNotFoundException);
    EXPECT_EQ(readOf(manager, Path::parse("/docs/moved.txt")), "payload");

    manager.rename(Path::parse("/docs/moved.txt"), "final.txt", Timestamp::of(4));
    EXPECT_EQ(manager.storage().resolve(Path::parse("/docs/final.txt")).name(), "final.txt");

    EXPECT_THROW(manager.copyFile(Path::parse("/docs"), Path::parse("/nope"),
                                  Timestamp::of(5)),
                 NotAFileException);
}

TEST(FileManagerTest, VersionsAndRollback)
{
    FileManager manager(FileSize::fromKilobytes(1));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(0));
    manager.createFile(kNotes, "v1", ivan, Timestamp::of(1));
    manager.saveVersion(kNotes, "v2", Timestamp::of(2));
    manager.saveVersion(kNotes, "v3", Timestamp::of(3));

    EXPECT_EQ(manager.versionHistoryFor(kNotes).count(), 2u);
    manager.rollback(kNotes, 1, Timestamp::of(4));
    EXPECT_EQ(readOf(manager, kNotes), "v2");
    EXPECT_EQ(manager.versionHistoryFor(kNotes).count(), 3u);
}

TEST(FileManagerTest, SearchTagsAndJournal)
{
    FileManager manager(FileSize::fromKilobytes(1));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(0));
    manager.createFile(kNotes, "hello", ivan, Timestamp::of(1));
    manager.tag(kNotes, Tag("work"));
    manager.describe(kNotes, "weekly notes");

    EXPECT_EQ(manager.search(NameFilter("notes")).size(), 1u);
    EXPECT_EQ(manager.storage().findMetadata(kNotes)->description(), "weekly notes");
    EXPECT_GE(manager.journal().count(), 1u);
    EXPECT_EQ(manager.journal().byKind("create").size(), 2u);
}

TEST(FileManagerTest, BackupAndRestore)
{
    FileManager manager(FileSize::fromKilobytes(1));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(0));
    manager.createFile(kNotes, "original", ivan, Timestamp::of(1));
    manager.makeBackup();

    manager.write(kNotes, "changed", Timestamp::of(2));
    EXPECT_EQ(readOf(manager, kNotes), "changed");

    EXPECT_EQ(manager.restoreBackup(Timestamp::of(3)), 1u);
    EXPECT_EQ(readOf(manager, kNotes), "original");
}

TEST(ReportServiceTest, RendersTreeAndSummary)
{
    FileManager manager(FileSize::fromKilobytes(1));
    RegularUser ivan("ivan", "Ivan", Timestamp::of(0));
    manager.createDirectory(Path::parse("/docs"), ivan, Timestamp::of(1));
    manager.createFile(kNotes, "hello", ivan, Timestamp::of(2));

    const std::string tree = ReportService().treeText(manager.storage());
    EXPECT_NE(tree.find("/\n  docs\n    notes.txt\n"), std::string::npos);

    const std::string summary = ReportService().summaryLine(manager.storage());
    EXPECT_NE(summary.find("2 directories"), std::string::npos);
    EXPECT_NE(summary.find("1 files"), std::string::npos);

    const auto top = ReportService().topLargest(manager.storage(), 1);
    ASSERT_EQ(top.size(), 1u);
    EXPECT_EQ(top[0].first.toString(), "/docs/notes.txt");
}
