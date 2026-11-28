#include <gtest/gtest.h>
#include "set.h"
#include "set_element.h"
#include <sstream>
#include "set_parser.h"
#include <stdexcept>
#include <string>

TEST(SetElementTest, AtomHasNoNestedSet)
{
    SetElement element(std::string("a"));

    EXPECT_TRUE(element.isAtom());
    EXPECT_EQ(element.atom(), "a");
}

TEST(SetElementTest, CopyOwnsSeparateNestedSet)
{
    Set inner;
    SetElement original(inner);
    SetElement copy = original;

    EXPECT_FALSE(copy.isAtom());
    EXPECT_NE(&original.nested(), &copy.nested());
}

TEST(SetTest, AddIgnoresDuplicates)
{
    Set set;
    set.add(SetElement(std::string("a")));
    set.add(SetElement(std::string("a")));

    EXPECT_EQ(set.size(), 1u);
}

TEST(SetTest, EqualityIgnoresOrder)
{
    Set first;
    first.add(SetElement(std::string("a")));
    first.add(SetElement(std::string("b")));

    Set second;
    second.add(SetElement(std::string("b")));
    second.add(SetElement(std::string("a")));

    EXPECT_EQ(first, second);
}

TEST(SetTest, EqualityHandlesNestedSets)
{
    Set inner;
    inner.add(SetElement(std::string("a")));
    inner.add(SetElement(std::string("b")));

    Set first;
    first.add(SetElement(inner));

    Set second;
    second.add(SetElement(inner));

    EXPECT_EQ(first, second);
}

TEST(SetTest, PowerSetHasTwoToTheN)
{
    Set set;
    set.add(SetElement(std::string("a")));
    set.add(SetElement(std::string("b")));

    EXPECT_EQ(set.powerSet().size(), 4u);
}

TEST(SetTest, PowerSetOfEmptySetContainsEmptySet)
{
    Set empty;

    Set result = empty.powerSet();

    EXPECT_EQ(result.size(), 1u);
    EXPECT_TRUE(result.contains(SetElement(empty)));
}

TEST(SetTest, PowerSetOfSingletonHasTwoElements)
{
    Set set;
    set.add(SetElement(std::string("a")));

    EXPECT_EQ(set.powerSet().size(), 2u);
}

TEST(SetTest, WritesNestedStructure)
{
    Set inner;
    inner.add(SetElement(std::string("a")));

    Set set;
    set.add(SetElement(std::string("x")));
    set.add(SetElement(inner));

    std::ostringstream output;
    output << set;

    EXPECT_EQ(output.str(), "{x, {a}}");
}

TEST(SetTest, RemovesExistingElement)
{
    Set set;
    set.add(SetElement(std::string("a")));
    set.add(SetElement(std::string("b")));

    set.remove(SetElement(std::string("a")));

    EXPECT_EQ(set.size(), 1u);
    EXPECT_FALSE(set.contains(SetElement(std::string("a"))));
}

TEST(SetTest, RemoveIgnoresMissingElement)
{
    Set set;
    set.add(SetElement(std::string("a")));

    set.remove(SetElement(std::string("z")));

    EXPECT_EQ(set.size(), 1u);
}

TEST(SetTest, UnionCombinesElements)
{
    Set first = SetParser::parse("{a, b}");
    Set second = SetParser::parse("{b, c}");

    first += second;

    EXPECT_EQ(first.size(), 3u);
}

TEST(SetTest, IntersectionKeepsCommon)
{
    Set first = SetParser::parse("{a, b}");
    Set second = SetParser::parse("{b, c}");

    first *= second;

    EXPECT_EQ(first, SetParser::parse("{b}"));
}

TEST(SetTest, DifferenceRemovesCommon)
{
    Set first = SetParser::parse("{a, b}");
    Set second = SetParser::parse("{b, c}");

    first -= second;

    EXPECT_EQ(first, SetParser::parse("{a}"));
}

TEST(SetTest, BinaryOperatorsDoNotModifyOperands)
{
    Set first = SetParser::parse("{a, b}");
    Set second = SetParser::parse("{b, c}");

    EXPECT_EQ(first + second, SetParser::parse("{a, b, c}"));
    EXPECT_EQ(first * second, SetParser::parse("{b}"));
    EXPECT_EQ(first - second, SetParser::parse("{a}"));
    EXPECT_EQ(first, SetParser::parse("{a, b}"));
    EXPECT_EQ(second, SetParser::parse("{b, c}"));
}

TEST(SetTest, IndexOperatorChecksMembership)
{
    Set set = SetParser::parse("{a}");

    EXPECT_TRUE(set[SetElement(std::string("a"))]);
    EXPECT_FALSE(set[SetElement(std::string("z"))]);
}

TEST(SetTest, EmptySetIsEmpty)
{
    Set set;

    EXPECT_TRUE(set.isEmpty());
    EXPECT_EQ(set.size(), 0u);
}

TEST(SetTest, DifferentSizesAreNotEqual)
{
    EXPECT_NE(SetParser::parse("{a}"), SetParser::parse("{a, b}"));
}

TEST(SetTest, PowerSetThrowsOnLargeSet)
{
    Set set;
    for (int index = 0; index < 25; ++index)
    {
        set.add(SetElement(std::to_string(index)));
    }

    EXPECT_THROW(set.powerSet(), std::length_error);
}

TEST(SetElementTest, AssignmentCopiesNestedSet)
{
    Set inner = SetParser::parse("{a}");
    SetElement element(inner);
    SetElement other(std::string("x"));

    other = element;

    EXPECT_FALSE(other.isAtom());
    EXPECT_NE(&other.nested(), &element.nested());
}

TEST(SetElementTest, SelfAssignmentIsSafe)
{
    SetElement element(std::string("a"));

    element = element;

    EXPECT_EQ(element.atom(), "a");
}

TEST(SetElementTest, AtomIsNotEqualToNestedSet)
{
    Set inner;
    SetElement atom(std::string("a"));
    SetElement nested(inner);

    EXPECT_FALSE(atom == nested);
}

TEST(SetTest, SameSizeDifferentContentAreNotEqual)
{
    EXPECT_NE(SetParser::parse("{a}"), SetParser::parse("{b}"));
    EXPECT_NE(SetParser::parse("{a, b}"), SetParser::parse("{a, c}"));
}