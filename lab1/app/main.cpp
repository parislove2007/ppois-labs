#include "menu.h"
#include <iostream>

int main()
{
    Menu menu(std::cin, std::cout);
    menu.run();
    return 0;
}