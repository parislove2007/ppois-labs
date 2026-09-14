#include <gtest/gtest.h>
#include "rectangle.h"
#include "point.h"

TEST(PointTest, StoresCoordinates)
{
    Point point(3, 5);
    EXPECT_EQ(point.x(), 3);
    EXPECT_EQ(point.y(), 5);
}

TEST(PointTest, EqualityComparesCoordinates)
{
    Point first(3, 5);
    Point same(3, 5);
    Point different(3, 6);

    EXPECT_EQ(first, same);
    EXPECT_NE(first, different);
}
