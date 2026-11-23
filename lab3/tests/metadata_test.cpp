#include <gtest/gtest.h>

#include <string>

#include "error/EntryAlreadyExistsException.h"
#include "error/InvalidFileNameException.h"
#include "error/PathNotFoundException.h"
#include "error/VersionConflictException.h"
#include "metadata/EntryVersion.h"
#include "metadata/FileMetadata.h"
#include "metadata/Tag.h"
#include "metadata/VersionHistory.h"
#include "value/Timestamp.h"

using namespace fs;

TEST(TagTest, StoresLabelAndRejectsEmpty)
{
    Tag tag("work");
    EXPECT_EQ(tag.label(), "work");
    EXPECT_EQ(tag, Tag("work"));
    EXPECT_THROW(Tag(""), InvalidFileNameException);
}

TEST(EntryVersionTest, KeepsSnapshotAndChecksum)
{
    EntryVersion version(2, "data", Timestamp::of(42));
    EXPECT_EQ(version.number(), 2);
    EXPECT_EQ(version.content(), "data");
    EXPECT_EQ(version.saved().seconds(), 42);
    EXPECT_TRUE(version.checksum().verify("data"));
}

TEST(VersionHistoryTest, NumbersVersionsConsecutively)
{
    VersionHistory history;
    EXPECT_EQ(history.count(), 0u);
    EXPECT_THROW(history.latest(), VersionConflictException);

    history.addVersion("first", Timestamp::of(1));
    history.addVersion("second", Timestamp::of(2));
    EXPECT_EQ(history.count(), 2u);
    EXPECT_EQ(history.latest().number(), 2);
    EXPECT_EQ(history.latest().content(), "second");
    EXPECT_EQ(history.findByNumber(1).content(), "first");
    EXPECT_THROW(history.findByNumber(9), VersionConflictException);
}

TEST(VersionHistoryTest, RollbackAddsNewVersionWithOldContent)
{
    VersionHistory history;
    history.addVersion("original", Timestamp::of(1));
    history.addVersion("broken", Timestamp::of(2));

    const EntryVersion rollback = history.rollbackTo(1, Timestamp::of(3));
    EXPECT_EQ(rollback.number(), 3);
    EXPECT_EQ(rollback.content(), "original");
    EXPECT_EQ(history.count(), 3u);
    EXPECT_THROW(history.rollbackTo(99, Timestamp::of(4)), VersionConflictException);
}

TEST(FileMetadataTest, ManagesTags)
{
    FileMetadata metadata;
    EXPECT_TRUE(metadata.description().empty());
    metadata.setDescription("weekly report");
    EXPECT_EQ(metadata.description(), "weekly report");

    metadata.addTag(Tag("work"));
    metadata.addTag(Tag("important"));
    EXPECT_EQ(metadata.tagCount(), 2u);
    EXPECT_TRUE(metadata.hasTag("work"));
    EXPECT_THROW(metadata.addTag(Tag("work")), EntryAlreadyExistsException);

    metadata.removeTag("work");
    EXPECT_FALSE(metadata.hasTag("work"));
    EXPECT_THROW(metadata.removeTag("work"), PathNotFoundException);
}
