#include "menu.h"
#include "set_parser.h"
#include <iostream>
#include <string>
#include <stdexcept>

Menu::Menu(std::istream& input, std::ostream& output)
    : input_(input), output_(output), rectangle_({0, 0}, {1, 1}) {}

void Menu::run()
{
    while (true)
    {
        showMainMenu();

        int choice = 0;
        if (!(input_ >> choice))
        {
            return;
        }

        if (choice == 0)
        {
            return;
        }
        else if (choice == 1)
        {
            runRectangleMenu();
        }
        else if (choice == 2)
        {
            runSetMenu();
        }
        else
        {
            output_ << "Неизвестный пункт\n";
        }
    }
}

void Menu::showMainMenu()
{
    output_ << "\n1. Прямоугольник\n2. Множество\n0. Выход\nВыбор: ";
}

void Menu::runRectangleMenu()
{
    while (true)
    {
        output_ << "\n--- Прямоугольник ---\n";
        output_ << "Текущий: " << rectangle_ << "\n";
        output_ << "1. Задать координаты\n2. Переместить\n3. Изменить размер\n";
        output_ << "4. Увеличить на единицу\n5. Уменьшить на единицу\n";
        output_ << "6. Объединить с другим\n7. Пересечь с другим\n0. Назад\nВыбор: ";

        int choice = 0;
        if (!(input_ >> choice) || choice == 0)
        {
            return;
        }

        try
        {
            if (choice == 1)
            {
                int left = 0, bottom = 0, right = 0, top = 0;
                output_ << "Введите left bottom right top: ";
                input_ >> left >> bottom >> right >> top;
                rectangle_ = Rectangle({left, bottom}, {right, top});
            }
            else if (choice == 2)
            {
                int deltaX = 0, deltaY = 0;
                output_ << "Введите сдвиг по x и y: ";
                input_ >> deltaX >> deltaY;
                rectangle_.move(deltaX, deltaY);
            }
            else if (choice == 3)
            {
                int width = 0, height = 0;
                output_ << "Введите ширину и высоту: ";
                input_ >> width >> height;
                rectangle_.resize(width, height);
            }
            else if (choice == 4)
            {
                ++rectangle_;
            }
            else if (choice == 5)
            {
                --rectangle_;
            }
            else if (choice == 6 || choice == 7)
            {
                int left = 0, bottom = 0, right = 0, top = 0;
                output_ << "Введите второй прямоугольник (left bottom right top): ";
                input_ >> left >> bottom >> right >> top;
                Rectangle other({left, bottom}, {right, top});
                rectangle_ = (choice == 6) ? rectangle_ + other : rectangle_ - other;
            }
            else
            {
                output_ << "Неизвестный пункт\n";
            }
        }
        catch (const std::exception& error)
        {
            output_ << "Ошибка: " << error.what() << "\n";
        }
    }
}

void Menu::runSetMenu()
{
    while (true)
    {
        output_ << "\n--- Множество ---\n";

        output_ << "Текущее: " << set_ << "\n";
        output_ << "1. Задать из строки\n2. Добавить элемент\n3. Удалить элемент\n";
        output_ << "4. Мощность\n5. Проверить принадлежность\n";
        output_ << "6. Объединение\n7. Пересечение\n8. Разность\n9. Булеан\n0. Назад\nВыбор: ";

        int choice = 0;
        if (!(input_ >> choice) || choice == 0)
        {
            return;
        }
        try
        {
            if (choice == 1)
            {
                std::string text;
                output_ << "Введите множество, например {a, b, {c}}: ";
                input_ >> std::ws;
                std::getline(input_, text);
                set_ = SetParser::parse(text);
            }
            else if (choice == 2||choice == 3 || choice == 5)
            {
                std::string atom;
                output_ << "Введите элемент: ";
                input_ >> atom;
                SetElement element(atom);

                if (choice==2)
                {
                    set_.add(element);
                }
                else if (choice==3)
                {
                    set_.remove(element);
                }
                else
                {
                    output_ << (set_[element] ? "Принадлежит\n" : "Не принадлежит\n");
                }
            }
            else if (choice == 4)
            {
                output_ << "Мощность: " << set_.size() << "\n";
            }
            else if (choice >= 6 && choice <= 8)
            {
                std::string text;
                output_ << "Введите второе множество: ";
                input_ >> std::ws;
                std::getline(input_, text);
                Set other = SetParser::parse(text);

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
            else if (choice == 9)
            {
                output_<<"Булеан: "<<set_.powerSet()<<"\n";
            }
            else
            {
                output_<<"Неизвестный пункт\n";
            }
        }
        catch (const std::exception& error)
        {
            output_<<"Ошибка: "<<error.what()<<"\n";
        }
    }
}