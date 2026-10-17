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