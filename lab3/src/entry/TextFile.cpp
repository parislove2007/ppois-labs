#include "entry/TextFile.h"

namespace fs
{

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
