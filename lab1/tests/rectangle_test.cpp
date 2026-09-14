#include <gtest/gtest.h>
#include "rectangle.h"

TEST(PointTest, StoresCoordinates)
{
    Point point(3, 5);
    EXPECT_EQ(point.x(), 3);
    EXPECT_EQ(point.y(), 5);
}