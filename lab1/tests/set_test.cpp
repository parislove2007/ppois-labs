#include <gtest/gtest.h>
#include "set.h"
#include "set_element.h"

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