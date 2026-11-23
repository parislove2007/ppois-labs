#include <iostream>

#include "ConsoleMenu.h"
#include "value/FileSize.h"

int main()
{
    fs::FileManager manager(fs::FileSize::fromKilobytes(1024));
    fs::ConsoleMenu menu(std::cin, std::cout, manager);
    menu.run();
    return 0;
}
