#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "error/InvalidFileNameException.h"
#include "error/InvalidFileSizeException.h"
#include "value/Checksum.h"
#include "value/FileExtension.h"
#include "value/FileSize.h"
#include "value/Path.h"
#include "value/Permission.h"
#include "value/Timestamp.h"

using namespace fs;

namespace
{

template <typename Value>
std::string printed(const Value& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}

bool throwsInvalidName(const std::string& raw)
{
    try
    {
        Path::parse(raw);
        return false;
    }
    catch (const InvalidFileNameException&)
    {
        return true;
    }
}

} // namespace

TEST(PathTest, ParsesAbsolutePath)
{
    const Path path = Path::parse("/docs/report.txt");
    EXPECT_EQ(path.toString(), "/docs/report.txt");
    EXPECT_TRUE(path.isAbsolute());
    EXPECT_EQ(path.name(), "report.txt");
    EXPECT_EQ(path.extension(), "txt");
}

TEST(PathTest, ParsesRelativePath)
{
    const Path path = Path::parse("docs/notes");
    EXPECT_FALSE(path.isAbsolute());
    EXPECT_EQ(path.name(), "notes");
}

TEST(PathTest, RootIsItsOwnParent)
{
    const Path root = Path::parse("/");
    EXPECT_EQ(root.name(), "/");
    EXPECT_EQ(root.parent().toString(), "/");
    EXPECT_TRUE(root.isAbsolute());
    EXPECT_TRUE(root.segments().empty());
}

TEST(PathTest, ParentDropsLastSegment)
{
    EXPECT_EQ(Path::parse("/a/b/c").parent().toString(), "/a/b");
    EXPECT_EQ(Path::parse("/a").parent().toString(), "/");
}

TEST(PathTest, SegmentsSplitPath)
{
    const std::vector<std::string> segments = Path::parse("/a/b/c.txt").segments();
    EXPECT_EQ(segments, (std::vector<std::string>{"a", "b", "c.txt"}));
}

TEST(PathTest, JoinAppendsChild)
{
    EXPECT_EQ(Path::parse("/docs").join(Path::parse("report.txt")).toString(),
              "/docs/report.txt");
    EXPECT_EQ(Path::parse("/").join(Path::parse("docs")).toString(), "/docs");
    EXPECT_EQ(Path::parse("docs").join(Path::parse("a/b")).toString(), "docs/a/b");
}

TEST(PathTest, RejectsBrokenPaths)
{
    EXPECT_TRUE(throwsInvalidName(""));
    EXPECT_TRUE(throwsInvalidName("/docs/"));
    EXPECT_TRUE(throwsInvalidName("/a//b"));
    EXPECT_TRUE(throwsInvalidName("/a/../b"));
    EXPECT_TRUE(throwsInvalidName("."));
}

TEST(PathTest, JoinRejectsAbsoluteChild)
{
    ASSERT_THROW(Path::parse("/docs").join(Path::parse("/abs")), InvalidFileNameException);
}

TEST(PathTest, ComparesByValue)
{
    EXPECT_EQ(Path::parse("/a/b"), Path::parse("/a/b"));
    EXPECT_EQ(printed(Path::parse("/a/b")), "/a/b");
}

TEST(FileSizeTest, StoresBytesAndKilobytes)
{
    EXPECT_EQ(FileSize::fromBytes(500).bytes(), 500u);
    EXPECT_EQ(FileSize::fromKilobytes(2).bytes(), 2048u);
    EXPECT_TRUE(FileSize().isZero());
}

TEST(FileSizeTest, RejectsNegativeSizes)
{
    EXPECT_THROW(FileSize::fromBytes(-1), InvalidFileSizeException);
    EXPECT_THROW(FileSize::fromKilobytes(-3), InvalidFileSizeException);
}

TEST(FileSizeTest, ArithmeticAndComparison)
{
    const FileSize a = FileSize::fromBytes(700);
    const FileSize b = FileSize::fromBytes(300);
    EXPECT_EQ((a + b).bytes(), 1000u);
    EXPECT_EQ((a - b).bytes(), 400u);
    EXPECT_TRUE(b < a);
    EXPECT_TRUE(a.fitsIn(FileSize::fromBytes(700)));
    EXPECT_FALSE(a.fitsIn(b));
    EXPECT_THROW(a - FileSize::fromBytes(701), InvalidFileSizeException);
}

TEST(FileSizeTest, PrintsHumanReadableUnits)
{
    EXPECT_EQ(printed(FileSize::fromBytes(512)), "512 B");
    EXPECT_EQ(printed(FileSize::fromKilobytes(2)), "2 KiB");
}

TEST(PermissionTest, FactoryPresets)
{
    EXPECT_EQ(Permission::none().toString(), "---");
    EXPECT_EQ(Permission::readOnly().toString(), "r--");
    EXPECT_EQ(Permission::readWrite().toString(), "rw-");
    EXPECT_EQ(Permission::full().toString(), "rwx");
}

TEST(PermissionTest, WithBuildersAddBits)
{
    const Permission custom = Permission::none().withRead().withExecute();
    EXPECT_EQ(custom.toString(), "r-x");
    EXPECT_TRUE(custom.canRead());
    EXPECT_FALSE(custom.canWrite());
}

TEST(PermissionTest, AllowsChecksRequiredBits)
{
    EXPECT_TRUE(Permission::readWrite().allows(Permission::readOnly()));
    EXPECT_FALSE(Permission::readOnly().allows(Permission::readWrite()));
    EXPECT_TRUE(Permission::full().allows(Permission::full()));
    EXPECT_EQ(printed(Permission::readOnly()), "r--");
}

TEST(TimestampTest, StoresAndShiftsSeconds)
{
    const Timestamp start = Timestamp::of(1000);
    EXPECT_EQ(start.seconds(), 1000);
    EXPECT_EQ(start.plusSeconds(60).seconds(), 1060);
    EXPECT_EQ(start.plusSeconds(-10).seconds(), 990);
}

TEST(TimestampTest, ComparesAndDiffs)
{
    const Timestamp early = Timestamp::of(100);
    const Timestamp late = Timestamp::of(250);
    EXPECT_TRUE(early.isBefore(late));
    EXPECT_TRUE(late.isAfter(early));
    EXPECT_EQ(late.diffSeconds(early), 150);
    EXPECT_EQ(printed(early), "100s");
}

TEST(ChecksumTest, IsDeterministic)
{
    EXPECT_EQ(Checksum::calculate("hello"), Checksum::calculate("hello"));
    EXPECT_NE(Checksum::calculate("hello"), Checksum::calculate("world"));
    EXPECT_EQ(Checksum(), Checksum::calculate(""));
}

TEST(ChecksumTest, VerifiesContent)
{
    const Checksum sum = Checksum::calculate("payload");
    EXPECT_TRUE(sum.verify("payload"));
    EXPECT_FALSE(sum.verify("payload "));
    EXPECT_EQ(sum.value().size(), 16u);
    EXPECT_EQ(printed(sum), sum.value());
}

TEST(FileExtensionTest, ExtractsAndLowercases)
{
    EXPECT_EQ(FileExtension::fromFileName("report.txt").value(), "txt");
    EXPECT_EQ(FileExtension::fromFileName("ARCHIVE.TXT").value(), "txt");
    EXPECT_TRUE(FileExtension::fromFileName("noext").isEmpty());
    EXPECT_TRUE(FileExtension::fromFileName(".hidden").isEmpty());
}

TEST(FileExtensionTest, KnowsCategories)
{
    EXPECT_TRUE(FileExtension::fromFileName("a.md").isText());
    EXPECT_TRUE(FileExtension::fromFileName("b.png").isImage());
    EXPECT_TRUE(FileExtension::fromFileName("c.zip").isArchive());
    EXPECT_FALSE(FileExtension::fromFileName("d.xyz").isKnown());
    EXPECT_EQ(printed(FileExtension::fromFileName("a.txt")), "txt");
}
