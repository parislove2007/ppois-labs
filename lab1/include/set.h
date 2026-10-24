#pragma once
#include <cstddef>
#include <vector>
#include "set_element.h"
#include <iosfwd>

class Set
{
public:
    Set();
    bool isEmpty() const;
    std::size_t size() const ;
    bool contains(const SetElement& element) const;
    bool operator==(const Set& other) const;
    void add(const SetElement& element);
    void remove(const SetElement& element);
    Set& operator+=(const Set& other);
    Set operator+(const Set& other) const;
    Set& operator*=(const Set& other);
    Set operator*(const Set& other) const;
    Set& operator-=(const Set& other);
    Set operator-(const Set& other) const;
    bool operator[](const SetElement& element) const;
    Set powerSet() const;
    const SetElement& at(std::size_t index) const;


private:
    std::vector<SetElement> elements_;
    static const std::size_t maxPowerSetSource = 20;
    
};

std::ostream& operator<<(std::ostream& out, const Set& set);
