#include "set_element.h"
#include "set.h"
#include <ostream>

SetElement::SetElement(const std::string& atom)
    : atom_(atom), nested_(nullptr) {}

SetElement::SetElement(const Set& nested)
    : atom_(""), nested_(new Set(nested)) {}

SetElement::SetElement(const SetElement& other)
    : atom_(other.atom_), nested_(nullptr)
{
    if (!other.isAtom())
    {
        nested_ = new Set(*other.nested_);
    }
}

SetElement& SetElement::operator=(const SetElement& other)
{
    if (this == &other)
    {
        return *this;
    }

    delete nested_;
    nested_ = nullptr;
    atom_ = other.atom_;

    if (!other.isAtom())
    {
        nested_ = new Set(*other.nested_);
    }

    return *this;
}

bool SetElement::operator==(const SetElement& other) const
{
    if(isAtom() != other.isAtom())
    {
        return false;
    }
    if(isAtom())
    {
        return atom_ == other.atom_;
    }

    return *nested_ == *other.nested_;
}

SetElement::~SetElement()
{
    delete nested_;
}

bool SetElement::isAtom() const { return nested_ == nullptr; }

const std::string& SetElement::atom() const { return atom_;}

const Set& SetElement::nested() const { return *nested_; }

std::ostream& operator<<(std::ostream& out, const SetElement& element)
{
    if (element.isAtom())
    {
        out << element.atom();
    }
    else
    {
        out << element.nested();   
    }
    return out;
}