#include "point.h"
#include<ostream>
#include<istream>


Point::Point(int x, int y) : x_(x), y_(y) {}
int Point:: x() const {return x_;}
int Point:: y() const {return y_;}
bool Point::operator==(const Point& other) const{ return x_ == other.x_ && y_ == other.y_;}
Point::Point(): x_(0),y_(0){}

std::ostream& operator<<(std::ostream& out, const Point& point)
{
    out<<point.x()<<" "<<point.y();
    return out;
}

std::istream& operator>>(std::istream& in, Point& point)
{
    int x = 0;
    int y = 0;
    in >> x >> y;    
    if(in)
    {
        point = Point(x,y);   
    }
    return in;
}