#pragma once

#include <string>

#include "value/Date.h"
#include "value/Money.h"
#include "value/PersonName.h"
#include "value/PhoneNumber.h"

namespace repair
{

/**
 * @brief Сотрудник сервисного центра.
 *
 * Абстрактный базовый класс. Должность и способ расчёта оклада
 * зависят от конкретной роли, поэтому объявлены виртуальными,
 * а создать сотрудника «вообще» нельзя.
 */
class Employee
{
public:
    /**
     * @brief Создаёт сотрудника.
     * @param name Фамилия, имя и отчество.
     * @param phone Контактный телефон.
     * @param hireDate Дата приёма на работу.
     * @param baseSalary Базовый оклад.
     */
    Employee(const PersonName& name, const PhoneNumber& phone, const Date& hireDate,
             const Money& baseSalary);

    /** @brief Виртуальный деструктор: класс предназначен для наследования. */
    virtual ~Employee();

    /** @return Фамилия, имя и отчество. */
    const PersonName& name() const;

    /** @return Контактный телефон. */
    const PhoneNumber& phone() const;

    /** @return Дата приёма на работу. */
    const Date& hireDate() const;

    /** @return Базовый оклад. */
    const Money& baseSalary() const;

    /** @return Название должности. */
    virtual std::string position() const = 0;

    /**
     * @brief Считает оклад с учётом надбавок, положенных должности.
     * @return Месячный оклад.
     */
    virtual Money monthlySalary() const;

    /**
     * @brief Считает стаж работы в сервисном центре.
     * @param today Дата, на которую считается стаж.
     * @return Полное число отработанных лет.
     */
    int yearsOfService(const Date& today) const;

    /**
     * @brief Проверяет право на надбавку за выслугу лет.
     * @param today Дата проверки.
     * @param requiredYears Необходимый стаж в годах.
     * @return true, если стаж достиг требуемого.
     */
    bool isEligibleForSeniorityBonus(const Date& today, int requiredYears) const;

private:
    PersonName name_;
    PhoneNumber phone_;
    Date hireDate_;
    Money baseSalary_;
};

}
