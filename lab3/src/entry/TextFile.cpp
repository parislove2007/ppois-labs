#include "entry/TextFile.h"

namespace fs
{

TextFile::TextFile(std::string name, std::string owner, Permission permission,
                   Timestamp created)
    : File(std::move(name), std::move(owner), permission, created),
      encoding_("utf-8"),
      lineEnding_("lf")
{
}

std::string TextFile::kind() const
{
    return "text";
}

int TextFile::lineCount() const
{
    if (content_.empty())
    {
        return 0;
    }
    int lines = 1;
    for (char ch : content_)
    {
        if (ch == '\n')
        {
            ++lines;
        }
    }
    return lines;
}

const std::string& TextFile::encoding() const
{
    return encoding_;
}

void TextFile::setEncoding(std::string encoding)
{
    encoding_ = std::move(encoding);
}

const std::string& TextFile::lineEnding() const
{
    return lineEnding_;
}

void TextFile::setLineEnding(std::string lineEnding)
{
    lineEnding_ = std::move(lineEnding);
}

int TextFile::wordCount() const
{
    int words = 0;
    bool inWord = false;
    for (char ch : content_)
    {
        const bool isSpace = ch == ' ' || ch == '\n' || ch == '\t';
        if (!isSpace && !inWord)
        {
            ++words;
        }
        inWord = !isSpace;
    }
    return words;
}

}
