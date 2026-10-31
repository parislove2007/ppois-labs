#include "set_parser.h"
#include <stdexcept>

std::string SetParser::parseAtom(const std::string& text, std::size_t& position)
{
    std::string atom;

    while (position < text.size() &&
           text[position] != ',' &&
           text[position] != '}' &&
           text[position] != ' ')
    {
        atom += text[position];
        ++position;
    }

    if (atom.empty())
    {
        throw std::invalid_argument("Пустой элемент множества");
    }

    return atom;
}

SetElement SetParser::parseElement(const std::string& text, std::size_t& position)
{
    skipSpaces(text, position);

    if (position < text.size() && text[position] == '{')
    {
        return SetElement(parseSet(text, position));
    }

    return SetElement(parseAtom(text, position));
}

void SetParser::skipSpaces(const std::string& text, std::size_t& position)
{
    while (position < text.size() && text[position] == ' ')
    {
        ++position;
    }
}

Set SetParser::parseSet(const std::string& text, std::size_t& position)
{
    skipSpaces(text, position);

    if (position >= text.size() || text[position] != '{')
    {
        throw std::invalid_argument("Ожидалась открывающая скобка");
    }
    ++position;

    Set result;
    skipSpaces(text, position);

    if (position < text.size() && text[position] == '}')
    {
        ++position;
        return result;
    }

    while (true)
    {
        result.add(parseElement(text, position));
        skipSpaces(text, position);

        if (position >= text.size())            

        {
            throw std::invalid_argument("Не хватает закрывающей скобки");
        }

        if (text[position] == ',')
        {
            ++position;
            continue;
        }

        if (text[position] == '}')
        {
            ++position;
            break;
        }

        throw std::invalid_argument("Ожидалась запятая или закрывающая скобка");
    }

    return result;
}

Set SetParser::parse(const std::string& text)
{
    std::size_t position = 0;
    Set result = parseSet(text, position);

    skipSpaces(text, position);

    if (position < text.size())
    {
        throw std::invalid_argument("Лишние символы после закрывающей скобки");
    }

    return result;
}