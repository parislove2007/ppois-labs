#pragma once
#include "point.h"

class Rectangle
{
    public:
    Rectangle(Point bottomLeft, Point topRight);
    int left() const;
    int right() const;
    int top() const;
    int bottom() const;
    Point bottomLeft() const;
    Point bottomRight() const;
    Point topRight() const;
    Point topLeft() const;
    
    private:

    Point bottomLeft_;
    Point topRight_;
    
    
};