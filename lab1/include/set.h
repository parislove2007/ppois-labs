#pragma once

#include <vector>
#include "set_element.h"

class Set
{
public:
    Set();
    bool isEmpty() const;
    std::size() const ;
    bool Contains(const SetElement& element) const 
    

private:
    std::vector<SetElement> elements_;
};