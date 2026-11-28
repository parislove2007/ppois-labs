#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "repair/Defect.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Результат диагностики устройства.
 *
 * Содержит перечень найденных неисправностей и заключение мастера.
 * Диагностика без найденных дефектов допустима: это заключение
 * «неисправность не подтвердилась».
 */
class Diagnostics
{
public:
    /**
     * @brief Создаёт результат диагностики.
     * @param performedBy Мастер, проводивший диагностику.
     * @param performedOn Дата проведения.
     * @param conclusion Заключение мастера.
     * @throws ValidationException Если мастер или заключение не указаны.
     */
    Diagnostics(const std::string& performedBy, const Date& performedOn,
                const std::string& conclusion);

    /** @return Мастер, проводивший диагностику. */
    const std::string& performedBy() const;

    /** @return Дата проведения. */
    const Date& performedOn() const;

    /** @return Заключение мастера. */
    const std::string& conclusion() const;

    /** @return Количество найденных неисправностей. */
    std::size_t defectCount() const;

    /**
     * @brief Добавляет найденную неисправность.
     *
     * Повторное добавление того же дефекта игнорируется.
     * @param defect Найденная неисправность.
     */
    void recordDefect(const Defect& defect);

    /** @return true, если найдена хотя бы одна неисправность. */
    bool requiresRepair() const;

    /**
     * @brief Находит самую критичную из найденных неисправностей.
     * @return Самая критичная неисправность.
     * @throws NotFoundException Если неисправностей не найдено.
     */
    const Defect& mostSevereDefect() const;

    /**
     * @brief Возвращает найденную неисправность по номеру.
     * @param index Номер неисправности, начиная с нуля.
     * @return Найденная неисправность.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const Defect& defectAt(std::size_t index) const;

    /**
     * @brief Считает множитель сложности ремонта по критичности дефектов.
     * @return Множитель, не меньше единицы.
     */
    int complexityFactor() const;

    /** @return Текст заключения для печати. */
    std::string describe() const;

private:
    std::string performedBy_;
    Date performedOn_;
    std::string conclusion_;
    std::vector<Defect> foundDefects_;
};

}
