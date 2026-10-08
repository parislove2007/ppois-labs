#include "rectangle.h"
#include<ostream>
#include<istream>
#include<algorithm>
#include <stdexcept>

Rectangle::Rectangle(Point bottomLeft, Point topRight) : bottomLeft_(bottomLeft),topRight_(topRight), isEmpty_(false) {}

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
    if (isEmpty_ || other.isEmpty_)
    return isEmpty_ && other.isEmpty_;
    return bottomLeft_ == other.bottomLeft_ && topRight_ == other.topRight_;
}

Rectangle& Rectangle:: operator++()
{
    if (isEmpty_) return *this; 
    Point newTopRight(topRight_.x() + 1, topRight_.y() + 1);
    topRight_ = newTopRight;
    return *this;
}

Rectangle Rectangle :: operator++(int) 
{
    Rectangle old = *this;
    ++(*this);
    return old;
}

Rectangle& Rectangle:: operator--()
{
    if (isEmpty_) return *this;
    Point newTopRight(topRight_.x() - 1, topRight_.y() - 1);
    

    if(newTopRight.x() < bottomLeft_.x() || newTopRight.y() < bottomLeft_.y())
        throw std::invalid_argument("Вы что-то перепутали...");

    topRight_ = newTopRight;

    return *this;
}

Rectangle Rectangle::operator--(int)
{
    Rectangle old = *this;
    --(*this);
    return old;
}

Rectangle Rectangle::operator-(const Rectangle& other) const
{
    Rectangle result = *this;
    result -= other;
    return result;
}

void Rectangle:: move(int deltaX, int deltaY)
    {
        if (isEmpty_) return;        
        Point newBottomLeft(bottomLeft_.x() + deltaX, bottomLeft_.y() + deltaY);
        Point newTopRight(topRight_.x() + deltaX, topRight_.y() + deltaY);
        bottomLeft_ = newBottomLeft;
        topRight_ = newTopRight;
    }
void Rectangle::resize(int width, int height)
    {
        if(isEmpty_) return;
        if (width < 0 || height < 0)
            {
                throw std::invalid_argument("Вы что-то перепутали...");
            }
        Point newTopRight(width + bottomLeft_.x(),height + bottomLeft_.y());
        topRight_ = newTopRight;
    }

std::ostream& operator<<(std::ostream& out, const Rectangle& rectangle)
{
    if (rectangle.isEmpty())
    {
    out << "empty";
    return out;
    }
    out<<rectangle.bottomLeft()<<" "<<rectangle.topRight();
    return out;
}

std::istream& operator>>(std::istream& in, Rectangle& rectangle)
{
    Point bottomLeft;
    Point topRight;
    in >> bottomLeft >> topRight;    
    if(in)
    {
           rectangle = Rectangle(bottomLeft,topRight);
    }
    return in;
}

Rectangle& Rectangle::operator+=(const Rectangle& other)
{
    if (other.isEmpty_)
    return *this;

    if (isEmpty_)
    {
    *this = other;
    return *this;
    }
    int newLeft = std::min(left(), other.left());
    int newBottom = std::min(bottom(), other.bottom());
    int newRight = std::max(right(), other.right());
    int newTop = std::max(top(), other.top());

    bottomLeft_ = Point(newLeft, newBottom);
    topRight_ = Point(newRight, newTop);

    return *this;
}

Rectangle& Rectangle::operator-=(const Rectangle& other)
{
    if (isEmpty_ || other.isEmpty_)
    {
        *this = Rectangle::empty();
        return *this;
    }

    int newLeft = std::max(left(), other.left());
    int newBottom = std::max(bottom(), other.bottom());
    int newRight = std::min(right(), other.right());
    int newTop = std::min(top(), other.top());

    if (newLeft > newRight || newBottom > newTop)
        *this = Rectangle::empty();
    else
    {
        bottomLeft_ = Point(newLeft, newBottom);
        topRight_ = Point(newRight, newTop);
    }

    return *this;
}

Rectangle Rectangle::operator+(const Rectangle& other) const
{
    Rectangle result = *this;
    result += other;
    return result;
}

Rectangle Rectangle::empty()
{
    Rectangle result({0,0}, {0,0});
    result.isEmpty_ = true;
    return result;
}

bool Rectangle::isEmpty() const { return isEmpty_; }

