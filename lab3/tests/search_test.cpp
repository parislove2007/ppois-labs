#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "entry/TextFile.h"
#include "metadata/Tag.h"
#include "search/CompositeFilter.h"
#include "search/DateFilter.h"
#include "search/ExtensionFilter.h"
#include "search/NameFilter.h"
#include "search/SearchService.h"
#include "search/SizeFilter.h"
#include "search/TagFilter.h"
#include "storage/Storage.h"
#include "value/FileExtension.h"
#include "value/FileSize.h"
#include "value/Timestamp.h"

using namespace fs;

namespace
{

Storage makeSample()
{
    Storage storage;
    storage.ensureDirectory(Path::parse("/docs"), "ivan", Timestamp::of(1));
    auto notes = std::make_unique<TextFile>("notes.txt", "ivan", Permission::readWrite(),
                                            Timestamp::of(2));
    notes->write("hello world", Timestamp::of(10));
    storage.attach(Path::parse("/docs/notes.txt"), std::move(notes), Timestamp::of(2));
    auto image = std::make_unique<TextFile>("photo.png", "ivan", Permission::readWrite(),
                                            Timestamp::of(3));
    image->write("12345678", Timestamp::of(20));
    storage.attach(Path::parse("/docs/photo.png"), std::move(image), Timestamp::of(3));
    return storage;
}

std::vector<std::string> pathsOf(const std::vector<Path>& found)
{
    std::vector<std::string> result;
    for (const Path& path : found)
    {
        result.push_back(path.toString());
    }
    return result;
}

} // namespace

TEST(SearchTest, NameFilterMatchesSubstring)
{
    Storage storage = makeSample();
    const auto found = SearchService().search(storage, NameFilter("notes"));
    EXPECT_EQ(pathsOf(found), (std::vector<std::string>{"/docs/notes.txt"}));
}

TEST(SearchTest, ExtensionFilterMatchesExtension)
{
    Storage storage = makeSample();
    const auto found = SearchService().search(
        storage, ExtensionFilter(FileExtension::fromFileName("x.png")));
    EXPECT_EQ(pathsOf(found), (std::vector<std::string>{"/docs/photo.png"}));
}

TEST(SearchTest, SizeFilterMatchesRange)
{
    Storage storage = makeSample();
    const auto found = SearchService().search(
        storage, SizeFilter(FileSize::fromBytes(9), FileSize::fromBytes(12)));
    EXPECT_EQ(pathsOf(found), (std::vector<std::string>{"/docs/notes.txt"}));
}

TEST(SearchTest, DateFilterMatchesModificationRange)
{
    Storage storage = makeSample();
    const auto recent =
        SearchService().search(storage, DateFilter(Timestamp::of(15), Timestamp::of(25)));
    EXPECT_EQ(pathsOf(recent), (std::vector<std::string>{"/docs/photo.png"}));
}

TEST(SearchTest, TagFilterUsesStorageMetadata)
{
    Storage storage = makeSample();
    storage.metadataFor(Path::parse("/docs/notes.txt")).addTag(Tag("work"));

    const auto tagged = SearchService().search(storage, TagFilter(storage, "work"));
    EXPECT_EQ(pathsOf(tagged), (std::vector<std::string>{"/docs/notes.txt"}));

    const auto missing = SearchService().search(storage, TagFilter(storage, "nope"));
    EXPECT_TRUE(missing.empty());
}

TEST(SearchTest, CompositeFilterCombinesPredicates)
{
    Storage storage = makeSample();
    CompositeFilter both(true);
    both.add(std::make_unique<NameFilter>("photo"));
    both.add(std::make_unique<ExtensionFilter>(FileExtension::fromFileName("x.png")));
    EXPECT_EQ(pathsOf(SearchService().search(storage, both)),
              (std::vector<std::string>{"/docs/photo.png"}));

    CompositeFilter strict(true);
    strict.add(std::make_unique<NameFilter>("photo"));
    strict.add(std::make_unique<NameFilter>("notes"));
    EXPECT_TRUE(SearchService().search(storage, strict).empty());

    CompositeFilter anyOf(false);
    anyOf.add(std::make_unique<NameFilter>("notes"));
    anyOf.add(std::make_unique<NameFilter>("photo"));
    EXPECT_EQ(SearchService().search(storage, anyOf).size(), 2u);
}

TEST(SearchTest, NameFilterIgnoresCaseWhenAllowed)
{
    Storage storage = makeSample();
    const auto strict = SearchService().search(storage, NameFilter("NOTES", true));
    EXPECT_TRUE(strict.empty());
    const auto loose = SearchService().search(storage, NameFilter("NOTES", false));
    EXPECT_EQ(pathsOf(loose), (std::vector<std::string>{"/docs/notes.txt"}));
}

TEST(SearchTest, DateFilterCanUseCreatedTimestamp)
{
    Storage storage = makeSample();
    const auto createdEarly =
        SearchService().search(storage, DateFilter(Timestamp::of(1), Timestamp::of(2), true));
    EXPECT_EQ(createdEarly.size(), 2u);
    const auto createdLate =
        SearchService().search(storage, DateFilter(Timestamp::of(4), Timestamp::of(5), true));
    EXPECT_TRUE(createdLate.empty());
}
