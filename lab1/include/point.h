#pragma once
class Point 
{   
    public:
    Point(int x,int y);
    int x() const;
    int y() const;
    private:
    int x_;
    int y_;

};