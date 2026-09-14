#include <gtest/gtest.h>
#include "rectangle.h"
#include <sstream>
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

TEST(RectangleTest, ReadsFromStream)
{
    std::istringstream input("3 5 10 12");
    Rectangle rectangle({0,0},{0,0});

    input >> rectangle;

    EXPECT_EQ(rectangle, Rectangle({3,5},{10,12}));
}

TEST(RectangleTest, WritesToStream)
{
    Rectangle rectangle({3,5},{11,15});
    std::ostringstream output;

    output << rectangle;

    EXPECT_EQ(output.str(), "3 5 11 15");
}

TEST(RectangleTest, FailsOnInvalidInput)
{
    std::istringstream input("2 abc");
    Rectangle rectangle({3,5},{10,12});
    input >> rectangle;
    EXPECT_TRUE(input.fail());
    EXPECT_EQ(rectangle,Rectangle({3,5},{10,12}));
}

TEST(RectangleTest, SurvivesRoundTrip)
{
    Rectangle original({3,5},{15,12});
    std::ostringstream output;
    output << original;

    std::istringstream input(output.str());
    Rectangle restored({0,0},{10,1});
    input >> restored;

    EXPECT_EQ(restored, original);
}

