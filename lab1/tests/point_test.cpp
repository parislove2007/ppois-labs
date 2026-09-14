#include <gtest/gtest.h>
#include "rectangle.h"
#include "point.h"
#include<sstream>


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

TEST(PointTest, WritesToStream)
{
    Point point(3, 5);
    std::ostringstream output;

    output << point;

    EXPECT_EQ(output.str(), "3 5");
}

TEST(PointTest, ReadsFromStream)
{
    std::istringstream input("3 5");
    Point point(0, 0);

    input >> point;

    EXPECT_EQ(point, Point(3, 5));
}

TEST(PointTest, FailsOnInvalidInput)
{
    std::istringstream input("abc");
    Point point(1, 2);

    input >> point;

    EXPECT_TRUE(input.fail());
    EXPECT_EQ(point, Point(1, 2));
}

TEST(PointTest, SurvivesRoundTrip)
{
    Point original(7, -3);
    std::ostringstream output;
    output << original;

    std::istringstream input(output.str());
    Point restored(0, 0);
    input >> restored;

    EXPECT_EQ(restored, original);
}