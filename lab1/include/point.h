#pragma once

#include <iosfwd>

class Point 
{   
    public:
    Point(int x,int y);
    int x() const;
    int y() const;
    bool operator==(const Point& other) const;
    Point();

    private:
    int x_;
    int y_;

};

    std::ostream& operator<<(std::ostream& out, const Point& point);
    std::istream& operator>>(std::istream& in, Point& point);


