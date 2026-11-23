#include "metadata/EntryVersion.h"

namespace fs
{

EntryVersion::EntryVersion(int number, std::string content, Timestamp saved)
    : number_(number), content_(std::move(content)), saved_(saved)
{
}

int EntryVersion::number() const
{
    return number_;
}

const std::string& EntryVersion::content() const
{
    return content_;
}

Timestamp EntryVersion::saved() const
{
    return saved_;
}

Checksum EntryVersion::checksum() const
{
    return Checksum::calculate(content_);
}

}
