#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "repair/Defect.h"
#include "repair/Device.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Акт приёмки устройства в ремонт.
 *
 * Фиксирует состояние устройства на момент приёмки: жалобу клиента и
 * внешние повреждения. Нужен, чтобы при выдаче не возникало спора о
 * том, какие царапины появились в мастерской.
 */
class IntakeAct
{
public:
    /**
     * @brief Создаёт акт приёмки.
     * @param device Принимаемое устройство.
     * @param declaredProblem Жалоба клиента своими словами.
     * @param acceptedOn Дата приёмки.
     * @param acceptedBy Сотрудник, принявший устройство.
     * @throws ValidationException Если жалоба или сотрудник не указаны.
     */
    IntakeAct(const Device& device, const std::string& declaredProblem,
              const Date& acceptedOn, const std::string& acceptedBy);

    /** @return Принятое устройство. */
    const Device& device() const;

    /** @return Жалоба клиента. */
    const std::string& declaredProblem() const;

    /** @return Дата приёмки. */
    const Date& acceptedOn() const;

    /** @return Сотрудник, принявший устройство. */
    const std::string& acceptedBy() const;

    /** @return Количество зафиксированных внешних повреждений. */
    std::size_t externalDefectCount() const;

    /**
     * @brief Фиксирует внешнее повреждение устройства.
     * @param defect Описание повреждения.
     */
    void recordExternalDefect(const Defect& defect);

    /**
     * @brief Проверяет наличие уже зафиксированного повреждения.
     * @param defect Проверяемое повреждение.
     * @return true, если такое повреждение уже записано.
     */
    bool hasExternalDefect(const Defect& defect) const;

    /** @return true, если есть повреждение, препятствующее ремонту. */
    bool hasBlockingDefect() const;

    /** @return Текст акта для печати. */
    std::string describe() const;

private:
    Device device_;
    std::string declaredProblem_;
    Date acceptedOn_;
    std::string acceptedBy_;
    std::vector<Defect> externalDefects_;
};

}
