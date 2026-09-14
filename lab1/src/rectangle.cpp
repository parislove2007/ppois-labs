#include "rectangle.h"

Rectangle::Rectangle(Point bottomLeft, Point topRight) : bottomLeft_(bottomLeft),topRight_(topRight) {}

int Rectangle::left() const {return bottomLeft_.x(); }
int Rectangle::right() const {return topRight_.x();}
int Rectangle::top() const {return topRight_.y(); }
int Rectangle::bottom() const {return bottomLeft_.y();}

Point Rectangle::bottomLeft() const {return bottomLeft_;}
Point Rectangle::topRight() const {return topRight_;}
Point Rectangle::bottomRight() const
{
    int rightX = topRight_.x();
    int bottomY = bottomLeft_.y();
    return Point(rightX,bottomY);
}
Point Rectangle::topLeft() const
{
    int leftX = bottomLeft_.x();
    int topY = topRight_.y();
    return Point(leftX,topY);
}

bool Rectangle::operator==(const Rectangle& other) const
{
    return (bottomLeft_ == other.bottomLeft_ && topRight_ == other.topRight_);
}
