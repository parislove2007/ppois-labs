#include "menu.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include "set_parser.h"

Menu::Menu(std::istream& input, std::ostream& output)
    : input_(input), output_(output), rectangle_({0, 0}, {1, 1}) {}

void Menu::run()
{
    while (true)
    {
        showMainMenu();
        int choice = 0;
        if (!(input_ >> choice) || choice == 0)
        {
            return;
        }
        switch (choice)
        {
            case 1: runRectangleMenu(); break;
            case 2: runSetMenu();       break;
            default: output_ << "Неизвестный пункт\n"; break;
        }
    }
}

void Menu::showMainMenu()
{
    output_ << "\n1. Прямоугольник\n2. Множество\n0. Выход\nВыбор: ";
}

void Menu::showRectangleMenu()
{
    output_ << "\n--- Прямоугольник ---\n";
    output_ << "Текущий: " << rectangle_ << "\n";
    output_ << "1. Задать координаты\n2. Переместить\n3. Изменить размер\n";
    output_ << "4. Увеличить на единицу\n5. Уменьшить на единицу\n";
    output_ << "6. Объединить с другим\n7. Пересечь с другим\n0. Назад\nВыбор: ";
}

void Menu::runRectangleMenu()
{
    while (true)
    {
        showRectangleMenu();
        int choice = 0;
        if (!(input_ >> choice) || choice == 0)
        {
            return;
        }
        try
        {
            handleRectangleChoice(choice);
        }
        catch (const std::exception& error)
        {
            output_ << "Ошибка: " << error.what() << "\n";
        }
    }
}

void Menu::handleRectangleChoice(int choice)
{
    switch (choice)
    {
        case 1: readRectangle();          break;
        case 2: moveRectangle();          break;
        case 3: resizeRectangle();        break;
        case 4: ++rectangle_;             break;
        case 5: --rectangle_;             break;
        case 6: combineRectangles(false); break;
        case 7: combineRectangles(true);  break;
        default: output_ << "Неизвестный пункт\n"; break;
    }
}

Rectangle Menu::readRectangleValue()
{
    int left = 0;
    int bottom = 0;
    int right = 0;
    int top = 0;
    input_ >> left >> bottom >> right >> top;
    return Rectangle({left, bottom}, {right, top});
}

void Menu::readRectangle()
{
    output_ << "Введите left bottom right top: ";
    rectangle_ = readRectangleValue();
}

void Menu::moveRectangle()
{
    int deltaX = 0;
    int deltaY = 0;
    output_ << "Введите сдвиг по x и y: ";
    input_ >> deltaX >> deltaY;
    rectangle_.move(deltaX, deltaY);
}

void Menu::resizeRectangle()
{
    int width = 0;
    int height = 0;
    output_ << "Введите ширину и высоту: ";
    input_ >> width >> height;
    rectangle_.resize(width, height);
}

void Menu::combineRectangles(bool intersect)
{
    output_ << "Введите второй прямоугольник (left bottom right top): ";
    Rectangle other = readRectangleValue();
    rectangle_ = intersect ? rectangle_ - other : rectangle_ + other;
}

void Menu::showSetMenu()
{
    output_ << "\n--- Множество ---\n";
    output_ << "Текущее: " << set_ << "\n";
    output_ << "1. Задать из строки\n2. Добавить элемент\n3. Удалить элемент\n";
    output_ << "4. Мощность\n5. Проверить принадлежность\n";
    output_ << "6. Объединение\n7. Пересечение\n8. Разность\n9. Булеан\n0. Назад\nВыбор: ";
}

void Menu::runSetMenu()
{
    while (true)
    {
        showSetMenu();
        int choice = 0;
        if (!(input_ >> choice) || choice == 0)
        {
            return;
        }
        try
        {
            handleSetChoice(choice);
        }
        catch (const std::exception& error)
        {
            output_ << "Ошибка: " << error.what() << "\n";
        }
    }
}

void Menu::handleSetChoice(int choice)
{
    switch (choice)
    {
        case 1: readSet();           break;
        case 2: addElement();        break;
        case 3: removeElement();     break;
        case 4: showCardinality();   break;
        case 5: checkMembership();   break;
        case 6:
        case 7:
        case 8: combineSets(choice); break;
        case 9: showPowerSet();      break;
        default: output_ << "Неизвестный пункт\n"; break;
    }
}

Set Menu::readSetValue()
{
    std::string text;
    input_ >> std::ws;
    std::getline(input_, text);
    return SetParser::parse(text);
}

SetElement Menu::readElement()
{
    std::string atom;
    output_ << "Введите элемент: ";
    input_ >> atom;
    return SetElement(atom);
}

void Menu::readSet()
{
    output_ << "Введите множество, например {a, b, {c}}: ";
    set_ = readSetValue();
}

void Menu::addElement()
{
    SetElement element = readElement();
    set_.add(element);
}

void Menu::removeElement()
{
    SetElement element = readElement();
    set_.remove(element);
}

void Menu::showCardinality()
{
    output_ << "Мощность: " << set_.size() << "\n";
}

void Menu::checkMembership()
{
    SetElement element = readElement();
    output_ << (set_[element] ? "Принадлежит\n" : "Не принадлежит\n");
}

void Menu::combineSets(int choice)
{
    output_ << "Введите второе множество: ";
    Set other = readSetValue();

    if (choice == 6)
    {
        set_ += other;
    }
    else if (choice == 7)
    {
        set_ *= other;
    }
    else
    {
        set_ -= other;
    }
}

void Menu::showPowerSet()
{
    Set result = set_.powerSet();
    output_ << "Булеан: " << result << "\n";
}