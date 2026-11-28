#include "value/FileExtension.h"

#include <algorithm>
#include <cctype>
#include <ostream>

namespace fs
{

namespace
{

std::string lowered(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return text;
}

} // namespace

FileExtension FileExtension::fromFileName(const std::string& fileName)
{
    const std::size_t dot = fileName.rfind('.');
    if (dot == std::string::npos || dot == 0 || dot + 1 == fileName.size())
    {
        return FileExtension("");
    }
    return FileExtension(lowered(fileName.substr(dot + 1)));
}

FileExtension::FileExtension() : value_("")
{
}

FileExtension::FileExtension(std::string value) : value_(std::move(value))
{
}

const std::string& FileExtension::value() const
{
    return value_;
}

bool FileExtension::isEmpty() const
{
    return value_.empty();
}

bool FileExtension::isText() const
{
    return value_ == "txt" || value_ == "md" || value_ == "csv" || value_ == "log";
}

bool FileExtension::isImage() const
{
    return value_ == "png" || value_ == "jpg" || value_ == "jpeg"
        || value_ == "gif" || value_ == "bmp";
}

bool FileExtension::isArchive() const
{
    return value_ == "zip" || value_ == "tar" || value_ == "gz" || value_ == "7z";
}

bool FileExtension::isKnown() const
{
    return isText() || isImage() || isArchive();
}

bool FileExtension::operator==(const FileExtension& other) const
{
    return value_ == other.value_;
}

std::ostream& operator<<(std::ostream& out, const FileExtension& extension)
{
    return out << extension.value();
}

}
