#pragma once

#include <iosfwd>
#include "rectangle.h"
#include "set.h"

class Menu
{
public:
    Menu(std::istream& input, std::ostream& output);

    void run();

private:
    void showMainMenu();
    void runRectangleMenu();
    void runSetMenu();

    std::istream& input_;
    std::ostream& output_;
    Rectangle rectangle_;
    Set set_;
};