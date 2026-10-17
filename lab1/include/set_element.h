#pragma once

#include <string>

class Set;  

class SetElement
{
public:
    explicit SetElement(const std::string& atom);
    explicit SetElement(const Set& nested);

    SetElement(const SetElement& other);              
    SetElement& operator=(const SetElement& other);   
    ~SetElement();                                    

    bool isAtom() const;
    const std::string& atom() const;
    const Set& nested() const;

    bool operator==(const SetElement& other) const;

private:
    std::string atom_;
    Set* nested_;      
};