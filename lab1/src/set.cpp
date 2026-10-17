#include "set.h"

Set::Set() {}


Set::operator==(const Set& other) const
{
    if(if elements_.size ()!= other.elements_.size())
    {
        return false;
    }

    for(const SetElement& element : elements_) 
    {
        if (!other.contains(element))
        return false;
    }
    return true;
}

SetElement::operator==(const SetElement& other) const
{
    if(isAtom() != other.IsAtom())
    {
        return false;
    }
    if(isAtom())
    {
        return atom_ == other.atom_;
    }

    return *nested_ == *other.nested_
}

int Set::size(const Set& elements) const
{
    return elements_.size();
}
bool Set:: isEmpty(const Set& elements) const 
{
    return elements_.empty();
}

bool Set::contains(const SetElement& element) const
{
    for (const SetElement& current : elements_)
    {
        if (current == element)
        {
            return true;
        }
    }
    return false;
}