#pragma once

#include <string>

#include "people/Employee.h"

namespace repair
{

/**
 * @brief Менеджер сервисного центра.
 *
 * Утверждает сметы и скидки в пределах установленного лимита.
 * Всё, что дороже лимита, требует решения вышестоящего менеджера.
 */
class Manager : public Employee
{
public:
    /**
     * @brief Создаёт менеджера.
     * @param name Фамилия, имя и отчество.
     * @param phone Контактный телефон.
     * @param hireDate Дата приёма на работу.
     * @param baseSalary Базовый оклад.
     * @param approvalLimit Предельная сумма, которую менеджер утверждает сам.
     */
    Manager(const PersonName& name, const PhoneNumber& phone, const Date& hireDate,
            const Money& baseSalary, const Money& approvalLimit);

    /** @return Название должности. */
    std::string position() const override;

    /**
     * @brief Считает оклад с надбавкой за число подчинённых.
     * @return Месячный оклад менеджера.
     */
    Money monthlySalary() const override;

    /** @return Предельная сумма самостоятельного утверждения. */
    const Money& approvalLimit() const;

    /** @return Количество подчинённых мастеров. */
    int subordinateCount() const;

    /**
     * @brief Проверяет право утвердить сумму самостоятельно.
     * @param amount Утверждаемая сумма.
     * @return true, если сумма не превышает лимит.
     */
    bool canApprove(const Money& amount) const;

    /** @brief Принимает ещё одного мастера под руководство. */
    void addSubordinate();

    /**
     * @brief Снимает одного мастера с подчинения.
     * @throws BusinessRuleException Если подчинённых нет.
     */
    void removeSubordinate();

private:
    Money approvalLimit_;
    int subordinateCount_;
};

}
