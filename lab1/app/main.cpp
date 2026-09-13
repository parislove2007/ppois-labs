#include<iostream>
#include"rectangle.h"
#include"point.h"

int main()
{
    Point a {4,6};
    Point b(10,18);
    Rectangle r (a,b);


    std::cout<<a.x()<<" "<<a.y()<<"\n";
    std::cout<<r.left()<<" "<<r.right()<<" "<<r.bottomRight().x()<<"\n";
    return 0;
}