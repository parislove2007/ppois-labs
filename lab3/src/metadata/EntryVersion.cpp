#include "metadata/EntryVersion.h"

namespace fs
{

EntryVersion::EntryVersion(int number, std::string content, Timestamp saved,
                             std::string author, std::string comment)
    : number_(number),
      content_(std::move(content)),
      saved_(saved),
      author_(std::move(author)),
      comment_(std::move(comment))
{
}

const std::string& EntryVersion::author() const
{
    return author_;
}

const std::string& EntryVersion::comment() const
{
    return comment_;
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
