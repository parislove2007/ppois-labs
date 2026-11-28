#include "value/Path.h"

#include <ostream>
#include <string>
#include <vector>

#include "error/InvalidFileNameException.h"

namespace fs
{

namespace
{

/** @brief Проверяет сегмент пути на допустимость. */
bool isValidSegment(const std::string& segment)
{
    return !segment.empty() && segment != "." && segment != "..";
}

} // namespace

Path Path::parse(const std::string& raw)
{
    if (raw.empty())
    {
        throw InvalidFileNameException("Path must not be empty");
    }
    if (raw != "/" && raw.back() == '/')
    {
        throw InvalidFileNameException("Path must not end with a separator: " + raw);
    }

    const bool absolute = raw.front() == '/';
    std::vector<std::string> segments;
    std::string segment;
    for (std::size_t i = 0; i < raw.size(); ++i)
    {
        const char ch = raw[i];
        if (ch != '/')
        {
            segment += ch;
            continue;
        }
        if (!segment.empty())
        {
            if (!isValidSegment(segment))
            {
                throw InvalidFileNameException("Invalid path segment: " + segment);
            }
            segments.push_back(segment);
            segment.clear();
        }
        else if (i != 0)
        {
            throw InvalidFileNameException("Empty path segment in: " + raw);
        }
    }
    if (!segment.empty())
    {
        if (!isValidSegment(segment))
        {
            throw InvalidFileNameException("Invalid path segment: " + segment);
        }
        segments.push_back(segment);
    }

    if (absolute && segments.empty())
    {
        return Path("/");
    }
    std::string value;
    for (const std::string& part : segments)
    {
        value += '/';
        value += part;
    }
    if (!absolute)
    {
        value.erase(0, 1);
    }
    return Path(value);
}

Path::Path() : value_("/")
{
}

Path::Path(std::string value) : value_(std::move(value))
{
}

const std::string& Path::toString() const
{
    return value_;
}

std::string Path::name() const
{
    if (value_ == "/")
    {
        return "/";
    }
    return value_.substr(value_.rfind('/') + 1);
}

Path Path::parent() const
{
    if (value_ == "/")
    {
        return Path();
    }
    const std::size_t lastSlash = value_.rfind('/');
    if (lastSlash == 0)
    {
        return Path();
    }
    return Path(value_.substr(0, lastSlash));
}

std::string Path::extension() const
{
    const std::string fileName = name();
    const std::size_t dot = fileName.rfind('.');
    if (dot == std::string::npos || dot == 0 || dot + 1 == fileName.size())
    {
        return "";
    }
    return fileName.substr(dot + 1);
}

bool Path::isAbsolute() const
{
    return !value_.empty() && value_.front() == '/';
}

std::vector<std::string> Path::segments() const
{
    std::vector<std::string> result;
    std::string segment;
    for (char ch : value_)
    {
        if (ch == '/')
        {
            if (!segment.empty())
            {
                result.push_back(segment);
                segment.clear();
            }
        }
        else
        {
            segment += ch;
        }
    }
    if (!segment.empty())
    {
        result.push_back(segment);
    }
    return result;
}

Path Path::join(const Path& child) const
{
    if (child.isAbsolute())
    {
        throw InvalidFileNameException("Joined path must be relative: " + child.toString());
    }
    if (value_ == "/")
    {
        return Path("/" + child.value_);
    }
    return Path(value_ + "/" + child.value_);
}

bool Path::operator==(const Path& other) const
{
    return value_ == other.value_;
}

std::ostream& operator<<(std::ostream& out, const Path& path)
{
    return out << path.toString();
}

}
