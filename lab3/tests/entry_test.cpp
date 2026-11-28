#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "entry/ArchiveFile.h"
#include "entry/BinaryFile.h"
#include "entry/Directory.h"
#include "entry/SymbolicLink.h"
#include "entry/TextFile.h"
#include "error/EntryAlreadyExistsException.h"
#include "error/FileSystemException.h"
#include "error/InvalidFileNameException.h"
#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"
#include "error/StorageFullException.h"
#include "value/Path.h"
#include "value/Permission.h"
#include "value/Timestamp.h"

using namespace fs;

namespace
{

std::unique_ptr<TextFile> makeTextFilePtr(const std::string& name)
{
    return std::make_unique<TextFile>(name, "ivan", Permission::readWrite(),
                                      Timestamp::of(100));
}

} // namespace

TEST(FileSystemEntryTest, RenameValidatesName)
{
    auto file = makeTextFilePtr("notes.txt");
    file->rename("diary.txt", Timestamp::of(200));
    EXPECT_EQ(file->name(), "diary.txt");
    EXPECT_EQ(file->modified().seconds(), 200);
    EXPECT_THROW(file->rename("", Timestamp::of(1)), InvalidFileNameException);
    EXPECT_THROW(file->rename("a/b", Timestamp::of(1)), InvalidFileNameException);
}

TEST(FileSystemEntryTest, ChangePermissionAndTouch)
{
    auto file = makeTextFilePtr("notes.txt");
    file->changePermission(Permission::readOnly());
    EXPECT_FALSE(file->permission().canWrite());
    file->touch(Timestamp::of(500));
    EXPECT_EQ(file->modified().seconds(), 500);
}

TEST(FileTest, WriteUpdatesContentChecksumAndTime)
{
    auto file = makeTextFilePtr("notes.txt");
    file->write("hello", Timestamp::of(150));
    EXPECT_EQ(file->read(), "hello");
    EXPECT_TRUE(file->checksum().verify("hello"));
    EXPECT_EQ(file->modified().seconds(), 150);
    EXPECT_EQ(file->size().bytes(), 5u);
}

TEST(FileTest, WriteRequiresWritePermission)
{
    TextFile locked("secret.txt", "ivan", Permission::readOnly(), Timestamp::of(1));
    EXPECT_THROW(locked.write("data", Timestamp::of(2)), PermissionDeniedException);
    EXPECT_TRUE(locked.read().empty());
}

TEST(TextFileTest, CountsLinesAndWords)
{
    TextFile text("poem.txt", "ivan", Permission::readWrite(), Timestamp::of(1));
    text.write("one two\nthree\n", Timestamp::of(2));
    EXPECT_EQ(text.lineCount(), 3);
    EXPECT_EQ(text.wordCount(), 3);
    EXPECT_EQ(text.describe(), "[text] poem.txt (14 bytes)");
}

TEST(TextFileTest, EmptyContentHasNoLines)
{
    TextFile text("empty.txt", "ivan", Permission::readWrite(), Timestamp::of(1));
    EXPECT_EQ(text.lineCount(), 0);
    EXPECT_EQ(text.wordCount(), 0);
}

TEST(BinaryFileTest, AppendGrowsContent)
{
    BinaryFile blob("data.bin", "ivan", Permission::readWrite(), Timestamp::of(1));
    blob.write("AB", Timestamp::of(2));
    blob.append("CD", Timestamp::of(3));
    EXPECT_EQ(blob.read(), "ABCD");
    EXPECT_TRUE(blob.checksum().verify("ABCD"));
    EXPECT_EQ(blob.describe(), "[binary] data.bin (4 bytes)");
}

TEST(ArchiveFileTest, PacksUniqueNames)
{
    ArchiveFile archive("backup.zip", "ivan", Permission::readWrite(), Timestamp::of(1));
    archive.packEntry("notes.txt", Timestamp::of(2));
    archive.packEntry("data.bin", Timestamp::of(3));
    EXPECT_EQ(archive.packedCount(), 2u);
    EXPECT_TRUE(archive.containsPacked("notes.txt"));
    EXPECT_THROW(archive.packEntry("notes.txt", Timestamp::of(4)),
                 EntryAlreadyExistsException);
}

TEST(DirectoryTest, OwnsEntriesAndRejectsDuplicates)
{
    Directory dir("docs", "ivan", Permission::readWrite(), Timestamp::of(1));
    dir.add(makeTextFilePtr("a.txt"), Timestamp::of(2));
    EXPECT_EQ(dir.entryCount(), 1u);
    EXPECT_TRUE(dir.contains("a.txt"));
    EXPECT_THROW(dir.add(makeTextFilePtr("a.txt"), Timestamp::of(3)),
                 EntryAlreadyExistsException);
}

TEST(DirectoryTest, RemoveHandsEntryBack)
{
    Directory dir("docs", "ivan", Permission::readWrite(), Timestamp::of(1));
    dir.add(makeTextFilePtr("a.txt"), Timestamp::of(2));
    std::unique_ptr<FileSystemEntry> taken = dir.remove("a.txt", Timestamp::of(3));
    EXPECT_EQ(taken->name(), "a.txt");
    EXPECT_EQ(dir.entryCount(), 0u);
    EXPECT_THROW(dir.remove("a.txt", Timestamp::of(4)), PathNotFoundException);
}

TEST(DirectoryTest, FindReturnsPolymorphicRef)
{
    Directory dir("docs", "ivan", Permission::readWrite(), Timestamp::of(1));
    dir.add(makeTextFilePtr("a.txt"), Timestamp::of(2));
    FileSystemEntry& found = dir.find("a.txt");
    EXPECT_FALSE(found.isDirectory());
    EXPECT_THROW(dir.find("missing.txt"), PathNotFoundException);
}

TEST(DirectoryTest, SizeSumsChildrenRecursively)
{
    Directory root("root", "ivan", Permission::readWrite(), Timestamp::of(1));
    auto docs = std::make_unique<Directory>("docs", "ivan", Permission::readWrite(),
                                            Timestamp::of(1));
    auto inner = makeTextFilePtr("inner.txt");
    inner->write("12345", Timestamp::of(2));
    docs->add(std::move(inner), Timestamp::of(2));
    auto outer = makeTextFilePtr("outer.txt");
    outer->write("123", Timestamp::of(2));
    root.add(std::move(docs), Timestamp::of(2));
    root.add(std::move(outer), Timestamp::of(2));
    EXPECT_EQ(root.size().bytes(), 8u);
    EXPECT_EQ(root.entryCount(), 2u);
    EXPECT_EQ(root.entries().size(), 2u);
    EXPECT_TRUE(root.isDirectory());
    EXPECT_EQ(root.describe(), "[dir] root (2 entries)");
}

TEST(SymbolicLinkTest, PointsToTargetAndRetargets)
{
    SymbolicLink link("shortcut", Path::parse("/docs/a.txt"), "ivan",
                      Permission::readOnly(), Timestamp::of(1));
    EXPECT_EQ(link.target().toString(), "/docs/a.txt");
    link.retarget(Path::parse("/docs/b.txt"), Timestamp::of(2));
    EXPECT_EQ(link.target().toString(), "/docs/b.txt");
    EXPECT_EQ(link.size().bytes(), 0u);
    EXPECT_EQ(link.describe(), "[link] shortcut -> /docs/b.txt");
}

TEST(TextFileTest, TracksEncodingAndLineEnding)
{
    TextFile text("a.txt", "ivan", Permission::readWrite(), Timestamp::of(1));
    EXPECT_EQ(text.encoding(), "utf-8");
    EXPECT_EQ(text.lineEnding(), "lf");
    text.setEncoding("cp1251");
    text.setLineEnding("crlf");
    EXPECT_EQ(text.encoding(), "cp1251");
    EXPECT_EQ(text.lineEnding(), "crlf");
}

TEST(BinaryFileTest, ReportsMimeType)
{
    BinaryFile blob("data.bin", "ivan", Permission::readWrite(), Timestamp::of(1));
    EXPECT_EQ(blob.mimeType(), "application/octet-stream");
}

TEST(ArchiveFileTest, ValidatesCompressionLevel)
{
    ArchiveFile archive("b.zip", "ivan", Permission::readWrite(), Timestamp::of(1));
    EXPECT_EQ(archive.compressionLevel(), 6);
    archive.setCompressionLevel(9);
    EXPECT_EQ(archive.compressionLevel(), 9);
    EXPECT_THROW(archive.setCompressionLevel(0), FileSystemException);
    EXPECT_THROW(archive.setCompressionLevel(10), FileSystemException);
    archive.setComment("nightly backup");
    EXPECT_EQ(archive.comment(), "nightly backup");
}

TEST(DirectoryTest, EnforcesEntryLimit)
{
    Directory dir("docs", "ivan", Permission::readWrite(), Timestamp::of(1), 1);
    dir.add(makeTextFilePtr("a.txt"), Timestamp::of(2));
    EXPECT_THROW(dir.add(makeTextFilePtr("b.txt"), Timestamp::of(3)),
                 StorageFullException);
}

TEST(SymbolicLinkTest, CountsFollows)
{
    SymbolicLink link("s", Path::parse("/a"), "ivan", Permission::readOnly(),
                      Timestamp::of(1));
    EXPECT_EQ(link.followCount(), 0);
    link.registerFollow();
    link.registerFollow();
    EXPECT_EQ(link.followCount(), 2);
}
