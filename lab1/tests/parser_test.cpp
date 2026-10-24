#include <gtest/gtest.h>
#include "set_parser.h"
#include <stdexcept>
#include <sstream>

TEST(SetParserTest, SurvivesRoundTrip)
{
    const std::string source = "{a, {b, c}, {}}";

    Set parsed = SetParser::parse(source);

    std::ostringstream output;
    output << parsed;

    EXPECT_EQ(output.str(), source);
}

TEST(SetParserTest, ParsesFlatSet)
{
    Set result = SetParser::parse("{a, b}");

    EXPECT_EQ(result.size(), 2u);
    EXPECT_TRUE(result.contains(SetElement(std::string("a"))));
}

TEST(SetParserTest, ParsesEmptySet)
{
    EXPECT_TRUE(SetParser::parse("{}").isEmpty());
}

TEST(SetParserTest, ParsesNestedSet)
{
    Set result = SetParser::parse("{a, {b, c}}");

    EXPECT_EQ(result.size(), 2u);
}

TEST(SetParserTest, ThrowsOnUnclosedBrace)
{
    EXPECT_THROW(SetParser::parse("{a, b"), std::invalid_argument);
}

TEST(SetParserTest, ThrowsOnMissingOpeningBrace)
{
    EXPECT_THROW(SetParser::parse("a, b}"), std::invalid_argument);
}

TEST(SetParserTest, ThrowsOnEmptyElement)
{
    EXPECT_THROW(SetParser::parse("{a,, b}"), std::invalid_argument);
}

TEST(SetParserTest, ThrowsOnTrailingGarbage)
{
    EXPECT_THROW(SetParser::parse("{a} x"), std::invalid_argument);
}