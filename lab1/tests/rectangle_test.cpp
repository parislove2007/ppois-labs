#include <gtest/gtest.h>
#include "rectangle.h"
#include "point.h"

TEST(RectangleTest, StoresCoordinates)
{
    Rectangle rectangle ({0,0},{10,5});
    
    EXPECT_EQ(rectangle.bottomLeft(),Point(0,0));
    EXPECT_EQ(rectangle.topLeft(),Point(0,5));
    EXPECT_EQ(rectangle.bottomRight(),Point(10,0));
}


TEST(RectangleTest, EqualityComparesBothCorners)
{
    Rectangle first{{0, 0}, {10, 5}};
    Rectangle same{{0, 0}, {10, 5}};
    Rectangle different{{0, 0}, {10, 6}};
    
    EXPECT_EQ(first, same);
    EXPECT_NE(first, different);
}