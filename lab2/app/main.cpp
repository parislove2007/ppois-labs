#include <iostream>

#include "ConsoleMenu.h"

int main()
{
    repair::ConsoleMenu menu(std::cin, std::cout);
    menu.run();
    return 0;
}
