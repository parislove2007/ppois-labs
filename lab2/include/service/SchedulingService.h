#pragma once

#include <memory>

#include "catalog/DeviceCategory.h"
#include "people/Technician.h"
#include "registry/EmployeeRegistry.h"
#include "repair/Estimate.h"
#include "schedule/TimeSlot.h"
#include "schedule/WorkSchedule.h"
#include "value/Date.h"
#include "value/Duration.h"

namespace repair
{

/**
 * @brief Подбор исполнителя и планирование работ.
 *
 * Сервис без состояния: все данные приходят параметрами. Это
 * позволяет планировать расписание разных мастеров одним и тем же
 * объектом сервиса.
 */
class SchedulingService
{
public:
    /** @brief Создаёт сервис планирования. */
    SchedulingService();

    /**
     * @brief Подбирает свободного мастера под категорию техники.
     * @param registry Реестр сотрудников.
     * @param category Категория техники.
     * @return Подходящий мастер.
     * @throws NotFoundException Если свободного мастера нет.
     */
    std::shared_ptr<Technician> pickTechnician(const EmployeeRegistry& registry,
                                               const DeviceCategory& category) const;

    /**
     * @brief Проверяет, можно ли занять интервал в графике.
     * @param schedule График мастера.
     * @param slot Планируемый интервал.
     * @return true, если интервал свободен.
     */
    bool canPlace(const WorkSchedule& schedule, const TimeSlot& slot) const;

    /**
     * @brief Занимает интервал в графике мастера.
     * @param schedule График мастера.
     * @param slot Занимаемый интервал.
     * @throws BusinessRuleException Если интервал занят.
     */
    void place(WorkSchedule& schedule, const TimeSlot& slot) const;

    /**
     * @brief Считает суммарное время работ по смете.
     * @param estimate Смета.
     * @return Суммарная длительность работ.
     */
    Duration plannedTime(const Estimate& estimate) const;

    /**
     * @brief Считает дату завершения работ при заданной загрузке.
     * @param start Дата начала.
     * @param work Суммарная длительность работ.
     * @param workdayMinutes Длительность рабочего дня в минутах.
     * @return Ожидаемая дата завершения.
     * @throws ValidationException Если рабочий день не положителен.
     */
    Date estimatedCompletion(const Date& start, const Duration& work,
                             int workdayMinutes) const;
};

}
