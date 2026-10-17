#pragma once

#include <vector>
#include "set_element.h"

class Set
{
public:
    Set();

private:
    std::vector<SetElement> elements_;
};