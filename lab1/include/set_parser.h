#pragma once

#include <cstddef>
#include <string>
#include "set.h"

class SetParser
{
public:
    static Set parse(const std::string& text);

private:
    static Set parseSet(const std::string& text, std::size_t& position);
    static SetElement parseElement(const std::string& text, std::size_t& position);
    static std::string parseAtom(const std::string& text, std::size_t& position);
    static void skipSpaces(const std::string& text, std::size_t& position);
};