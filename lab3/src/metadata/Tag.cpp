#include "metadata/Tag.h"

#include <ostream>

#include "error/InvalidFileNameException.h"

namespace fs
{

Tag::Tag(std::string label) : label_(std::move(label))
{
    if (label_.empty())
    {
        throw InvalidFileNameException("Tag label must not be empty");
    }
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
