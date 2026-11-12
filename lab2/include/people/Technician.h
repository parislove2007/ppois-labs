#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "catalog/DeviceCategory.h"
#include "people/Employee.h"

namespace repair
{

/**
 * @brief Мастер по ремонту.
 *
 * Обслуживает только те категории техники, по которым у него есть
 * специализация, и не может вести больше заказов, чем допускает
 * предел одновременной загрузки.
 */
class Technician : public Employee
{
public:
    /**
     * @brief Создаёт мастера.
     * @param name Фамилия, имя и отчество.
     * @param phone Контактный телефон.
     * @param hireDate Дата приёма на работу.
     * @param baseSalary Базовый оклад.
     * @param grade Разряд мастера, от первого до шестого.
     * @throws ValidationException Если разряд вне допустимого диапазона.
     */
    Technician(const PersonName& name, const PhoneNumber& phone, const Date& hireDate,
               const Money& baseSalary, int grade);

    /** @return Название должности. */
    std::string position() const override;

    /**
     * @brief Считает оклад с надбавкой за разряд.
     * @return Месячный оклад мастера.
     */
    Money monthlySalary() const override;

    /** @return Разряд мастера. */
    int grade() const;

    /** @return Количество заказов в работе. */
    int activeOrders() const;

    /** @return Количество освоенных категорий техники. */
    std::size_t specializationCount() const;

    /**
     * @brief Добавляет специализацию по категории техники.
     *
     * Повторное добавление той же категории игнорируется.
     * @param category Категория техники.
     */
    void addSpecialization(const DeviceCategory& category);

    /**
     * @brief Проверяет, может ли мастер обслуживать категорию техники.
     * @param category Категория техники.
     * @return true, если специализация есть.
     */
    bool canServe(const DeviceCategory& category) const;

    /** @return true, если мастер достиг предела одновременной загрузки. */
    bool isOverloaded() const;

    /**
     * @brief Берёт ещё один заказ в работу.
     * @throws BusinessRuleException Если предел загрузки уже достигнут.
     */
    void takeOrder();

    /**
     * @brief Снимает один заказ с мастера.
     * @throws BusinessRuleException Если в работе нет ни одного заказа.
     */
    void releaseOrder();

private:
    std::vector<DeviceCategory> specializations_;
    int grade_;
    int activeOrders_;
};

}
