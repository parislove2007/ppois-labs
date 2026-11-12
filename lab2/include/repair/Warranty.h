#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "value/Date.h"

namespace repair
{

/**
 * @brief Гарантия сервисного центра на выполненный ремонт.
 *
 * Покрывает только перечисленные узлы: если клиент вернётся с
 * неисправностью другого узла, это новый платный заказ, а не
 * гарантийный случай.
 */
class Warranty
{
public:
    /**
     * @brief Создаёт гарантию.
     * @param startDate Дата начала действия, обычно дата выдачи.
     * @param months Срок действия в месяцах.
     * @throws ValidationException Если срок не положителен.
     */
    Warranty(const Date& startDate, int months);

    /** @return Дата начала действия. */
    const Date& startDate() const;

    /** @return Срок действия в месяцах. */
    int months() const;

    /** @return Количество покрытых узлов. */
    std::size_t coveredUnitCount() const;

    /** @return Дата окончания действия гарантии. */
    Date expiryDate() const;

    /**
     * @brief Добавляет узел в зону покрытия.
     *
     * Повторное добавление того же узла игнорируется.
     * @param unit Название узла.
     * @throws ValidationException Если название пусто.
     */
    void addCoveredUnit(const std::string& unit);

    /**
     * @brief Проверяет, покрыт ли узел гарантией.
     * @param unit Название узла.
     * @return true, если узел входит в зону покрытия.
     */
    bool covers(const std::string& unit) const;

    /**
     * @brief Проверяет действие гарантии на дату.
     * @param date Дата проверки.
     * @return true, если гарантия действует.
     */
    bool isActiveOn(const Date& date) const;

    /**
     * @brief Считает, сколько дней гарантии осталось.
     * @param date Дата отсчёта.
     * @return Число оставшихся дней, не меньше нуля.
     */
    int daysLeftOn(const Date& date) const;

private:
    Date startDate_;
    int months_;
    std::vector<std::string> coveredUnits_;
};

}
