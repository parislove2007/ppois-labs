#include "metadata/Tag.h"

#include <ostream>

#include "error/InvalidFileNameException.h"

namespace fs
{

Tag::Tag(std::string label) : Tag(std::move(label), "general", "#9aa0a6")
{
}

Tag::Tag(std::string label, std::string category, std::string color)
    : label_(std::move(label)), category_(std::move(category)), color_(std::move(color))
{
    if (label_.empty())
    {
        throw InvalidFileNameException("Tag label must not be empty");
    }
}

const std::string& Tag::category() const
{
    return category_;
}

const std::string& Tag::color() const
{
    return color_;
}

const std::string& Tag::label() const
{
    return label_;
}

bool Tag::operator==(const Tag& other) const
{
    return label_ == other.label_;
}

std::ostream& operator<<(std::ostream& out, const Tag& tag)
{
    return out << tag.label();
}

}
