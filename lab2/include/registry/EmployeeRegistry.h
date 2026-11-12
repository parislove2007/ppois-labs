#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "catalog/DeviceCategory.h"
#include "people/Manager.h"
#include "people/Technician.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Реестр сотрудников сервисного центра.
 *
 * Мастера и менеджеры хранятся раздельно: подбор исполнителя идёт
 * только по мастерам, а утверждение смет — только по менеджерам,
 * и разделение избавляет от проверок типа во время поиска.
 */
class EmployeeRegistry
{
public:
    /** @brief Создаёт пустой реестр. */
    EmployeeRegistry();

    /** @return Количество мастеров. */
    std::size_t technicianCount() const;

    /** @return Количество менеджеров. */
    std::size_t managerCount() const;

    /**
     * @brief Принимает мастера на работу.
     * @param technician Принимаемый мастер.
     * @throws ValidationException Если мастер не задан.
     */
    void hire(const std::shared_ptr<Technician>& technician);

    /**
     * @brief Принимает менеджера на работу.
     * @param manager Принимаемый менеджер.
     * @throws ValidationException Если менеджер не задан.
     */
    void hire(const std::shared_ptr<Manager>& manager);

    /**
     * @brief Возвращает мастера по порядковому номеру.
     * @param index Номер, начиная с нуля.
     * @return Мастер.
     * @throws NotFoundException Если номер выходит за границы.
     */
    std::shared_ptr<Technician> technicianAt(std::size_t index) const;

    /**
     * @brief Возвращает менеджера по порядковому номеру.
     * @param index Номер, начиная с нуля.
     * @return Менеджер.
     * @throws NotFoundException Если номер выходит за границы.
     */
    std::shared_ptr<Manager> managerAt(std::size_t index) const;

    /**
     * @brief Находит незагруженного мастера с нужной специализацией.
     * @param category Категория техники.
     * @return Подходящий мастер.
     * @throws NotFoundException Если свободного мастера нет.
     */
    std::shared_ptr<Technician> findAvailableFor(const DeviceCategory& category) const;

    /**
     * @brief Находит менеджера, способного утвердить сумму.
     * @param amount Утверждаемая сумма.
     * @return Подходящий менеджер.
     * @throws NotFoundException Если такого менеджера нет.
     */
    std::shared_ptr<Manager> findApproverFor(const Money& amount) const;

    /** @return Суммарный месячный фонд оплаты труда. */
    Money monthlyPayroll() const;

private:
    std::vector<std::shared_ptr<Technician>> technicians_;
    std::vector<std::shared_ptr<Manager>> managers_;
};

}
