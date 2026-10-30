#include "set.h"
#include<algorithm>
#include <stdexcept>
#include <ostream>


Set::Set() {}


bool Set::operator==(const Set& other) const
{
    if( elements_.size ()!= other.elements_.size())
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

std::size_t Set::size() const { 
    return elements_.size();
 }

bool Set::isEmpty() const 
{
    return elements_.empty();
}

bool Set::contains(const SetElement& element) const
{
    return std::find(elements_.begin(), elements_.end(), element) != elements_.end();
}

void Set::add(const SetElement& element)
{
    if (contains(element))
    {
        return;
    }
    elements_.push_back(element);
}

void Set::appendUnchecked(const SetElement& element)
{
    elements_.push_back(element);
}

void Set::remove(const SetElement& element)
{
    auto position = std::find(elements_.begin(), elements_.end(), element);

    if (position != elements_.end())
    {
        elements_.erase(position);
    }
}

Set& Set::operator+=(const Set& other)
{
    for (const SetElement& element : other.elements_)
    {
        add(element);
    }
    return *this;
}

Set& Set::operator*=(const Set& other)
{
    std::vector<SetElement> common;

    for (const SetElement& element : elements_)
    {
        if (other.contains(element))
        {
            common.push_back(element);
        }
    }

    elements_ = common;
    return *this;
}

Set Set::operator+(const Set& other) const
{
    Set result = *this;
    result += other;
    return result;
}

Set& Set::operator-=(const Set& other)
{
    std::vector<SetElement> common;

    for (const SetElement& element : elements_)
    {
        if (!other.contains(element))
        {
            common.push_back(element);
        }
    }

    elements_ = common;
    return *this;
}

Set Set::operator*(const Set& other) const
{
    Set result = *this;
    result *= other;
    return result;
}

Set Set::operator-(const Set& other) const
{
    Set result = *this;
    result -= other;
    return result;
}

bool Set::operator[](const SetElement& element) const
{
    return contains(element);
}

Set Set::powerSet() const
{
    if (elements_.size() > maxPowerSetSource)
    {
        throw std::length_error("Error! Invalid length");
    }

    Set result;
    const std::size_t subsetCount = 1u << elements_.size();

    for (std::size_t mask = 0; mask < subsetCount; ++mask)
    {
        Set subset;

        for (std::size_t index = 0; index < elements_.size(); ++index)
        {
            if (mask & (1u << index))
            {
                subset.appendUnchecked(elements_[index]);
            }
        }

            result.appendUnchecked(SetElement(subset));
    }

    return result;
}

const SetElement& Set::at(std::size_t index) const{
    return elements_[index];
}

std::ostream& operator<<(std::ostream& out, const Set& set)
{
    out << "{";
    for (std::size_t index = 0; index < set.size(); ++index)
    {
        if (index > 0)
        {
            out << ", ";
        }
        out << set.at(index);
    }
    out << "}";
    return out;
}

