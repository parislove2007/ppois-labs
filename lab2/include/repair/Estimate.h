#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "repair/PartUsage.h"
#include "repair/WorkItem.h"
#include "value/Date.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Смета на ремонт: перечень работ и запчастей с итоговой суммой.
 *
 * После утверждения клиентом смета становится неизменной: добавить
 * работу или запчасть в утверждённую смету нельзя, нужно составлять
 * дополнительную.
 */
class Estimate
{
public:
    /** @brief Создаёт пустую неутверждённую смету. */
    Estimate();

    /** @return Количество позиций работ. */
    std::size_t workCount() const;

    /** @return Количество позиций запчастей. */
    std::size_t partCount() const;

    /** @return true, если смета утверждена клиентом. */
    bool isApproved() const;

    /** @return true, если в смете нет ни работ, ни запчастей. */
    bool isEmpty() const;

    /**
     * @brief Добавляет работу в смету.
     * @param work Выполняемая работа.
     * @throws BusinessRuleException Если смета уже утверждена.
     */
    void addWork(const WorkItem& work);

    /**
     * @brief Добавляет запчасть в смету.
     * @param usage Списываемая запчасть.
     * @throws BusinessRuleException Если смета уже утверждена.
     */
    void addPart(const PartUsage& usage);

    /** @return Суммарная стоимость работ. */
    Money worksCost() const;

    /** @return Суммарная стоимость запчастей. */
    Money partsCost() const;

    /** @return Итоговая сумма сметы. */
    Money totalCost() const;

    /**
     * @brief Утверждает смету от имени клиента.
     * @param approvedOn Дата утверждения.
     * @throws BusinessRuleException Если смета пуста или уже утверждена.
     */
    void approve(const Date& approvedOn);

    /**
     * @brief Возвращает дату утверждения.
     * @return Дата утверждения.
     * @throws EstimateNotApprovedException Если смета не утверждена.
     */
    const Date& approvalDate() const;

    /**
     * @brief Возвращает позицию работ по порядковому номеру.
     * @param index Номер позиции, начиная с нуля.
     * @return Позиция работ.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const WorkItem& workAt(std::size_t index) const;

    /**
     * @brief Возвращает позицию запчастей по порядковому номеру.
     * @param index Номер позиции, начиная с нуля.
     * @return Позиция запчастей.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const PartUsage& partAt(std::size_t index) const;

private:
    std::vector<WorkItem> works_;
    std::vector<PartUsage> parts_;
    std::optional<Date> approvedOn_;
};

}
