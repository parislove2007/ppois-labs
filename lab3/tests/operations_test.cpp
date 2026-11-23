#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "operations/CopyOperation.h"
#include "operations/CreateOperation.h"
#include "operations/DeleteOperation.h"
#include "operations/MoveOperation.h"
#include "operations/OperationJournal.h"
#include "operations/RenameOperation.h"
#include "value/Path.h"
#include "value/Timestamp.h"

using namespace fs;

TEST(OperationTest, EntriesKnowKindAndDescription)
{
    const CreateOperation create(Path::parse("/a.txt"), Timestamp::of(1));
    EXPECT_EQ(create.kind(), "create");
    EXPECT_EQ(create.describe(), "created /a.txt");
    EXPECT_EQ(create.at().seconds(), 1);

    const DeleteOperation remove(Path::parse("/a.txt"), Timestamp::of(2));
    EXPECT_EQ(remove.describe(), "deleted /a.txt");

    const MoveOperation move(Path::parse("/a.txt"), Path::parse("/b/a.txt"), Timestamp::of(3));
    EXPECT_EQ(move.describe(), "moved /a.txt -> /b/a.txt");
    EXPECT_EQ(move.from().toString(), "/a.txt");

    const CopyOperation copy(Path::parse("/a.txt"), Path::parse("/copy.txt"), Timestamp::of(4));
    EXPECT_EQ(copy.kind(), "copy");

    const RenameOperation rename(Path::parse("/a.txt"), "b.txt", Timestamp::of(5));
    EXPECT_EQ(rename.describe(), "renamed /a.txt to b.txt");
    EXPECT_EQ(rename.newName(), "b.txt");
}

TEST(OperationJournalTest, CollectsAndFiltersEntries)
{
    OperationJournal journal;
    EXPECT_EQ(journal.count(), 0u);

    journal.log(std::make_unique<CreateOperation>(Path::parse("/a"), Timestamp::of(1)));
    journal.log(std::make_unique<DeleteOperation>(Path::parse("/a"), Timestamp::of(2)));
    journal.log(std::make_unique<CreateOperation>(Path::parse("/b"), Timestamp::of(3)));

    EXPECT_EQ(journal.count(), 3u);
    EXPECT_EQ(journal.at(1).kind(), "delete");
    EXPECT_EQ(journal.byKind("create").size(), 2u);
    EXPECT_EQ(journal.byKind("rename").size(), 0u);
    EXPECT_NE(journal.historyText().find("created /b"), std::string::npos);
}
