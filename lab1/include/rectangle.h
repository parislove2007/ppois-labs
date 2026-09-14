#pragma once
#include "point.h"
#include <iosfwd>

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
    bool operator==(const Rectangle& other) const;
    void move(int deltaX, int deltaY);
    void resize(int width, int height);
    
    private:

    Point bottomLeft_;
    Point topRight_;
    
    
};

std::ostream& operator<<(std::ostream& out, const Rectangle& rectangle);
std::istream& operator>>(std::istream& in, Rectangle& rectangle);

