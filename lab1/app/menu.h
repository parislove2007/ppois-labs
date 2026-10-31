#pragma once

#include <iosfwd>
#include <string>
#include "rectangle.h"
#include "set.h"

/**
 * @brief Консольное меню для работы с прямоугольником и множеством.
 *
 * Потоки ввода и вывода передаются извне, поэтому меню не привязано
 * к консоли и может быть проверено на строковых потоках.
 */
class Menu
{
public:
    /**
     * @brief Создаёт меню, работающее с заданными потоками.
     * @param input Поток ввода.
     * @param output Поток вывода.
     */
    Menu(std::istream& input, std::ostream& output);

    /** @brief Запускает главный цикл меню. */
    void run();

private:
    void showMainMenu();
    void showRectangleMenu();
    void showSetMenu();

    void runRectangleMenu();
    void runSetMenu();

    void handleRectangleChoice(int choice);
    void handleSetChoice(int choice);

    Rectangle readRectangleValue();
    void readRectangle();
    void moveRectangle();
    void resizeRectangle();
    void combineRectangles(bool intersect);

    Set readSetValue();
    SetElement readElement();
    void readSet();
    void addElement();
    void removeElement();
    void showCardinality();
    void checkMembership();
    void combineSets(int choice);
    void showPowerSet();

    std::istream& input_;
    std::ostream& output_;
    Rectangle rectangle_;
    Set set_;
};