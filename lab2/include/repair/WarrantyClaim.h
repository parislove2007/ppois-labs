#pragma once

#include <string>

#include "repair/Warranty.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Обращение клиента по гарантии на выполненный ранее ремонт.
 *
 * Обращение рассматривается один раз: после принятия или отклонения
 * решение не меняется, а повторное обращение оформляется отдельно.
 */
class WarrantyClaim
{
public:
    /**
     * @brief Создаёт гарантийное обращение.
     * @param originalOrderNumber Номер исходного заказа.
     * @param failedUnit Узел, который снова вышел из строя.
     * @param submittedOn Дата обращения.
     * @throws ValidationException Если узел не указан или номер заказа не положителен.
     */
    WarrantyClaim(int originalOrderNumber, const std::string& failedUnit,
                  const Date& submittedOn);

    /** @return Номер исходного заказа. */
    int originalOrderNumber() const;

    /** @return Узел, вышедший из строя. */
    const std::string& failedUnit() const;

    /** @return Дата обращения. */
    const Date& submittedOn() const;

    /** @return true, если по обращению принято решение. */
    bool isResolved() const;

    /** @return true, если обращение признано гарантийным случаем. */
    bool isAccepted() const;

    /**
     * @brief Проверяет обращение на соответствие условиям гарантии.
     * @param warranty Гарантия по исходному заказу.
     * @return true, если гарантия действует и покрывает узел.
     */
    bool matches(const Warranty& warranty) const;

    /**
     * @brief Признаёт обращение гарантийным случаем.
     * @throws BusinessRuleException Если решение уже принято.
     */
    void accept();

    /**
     * @brief Отклоняет обращение.
     * @param reason Причина отказа.
     * @throws BusinessRuleException Если решение уже принято.
     * @throws ValidationException Если причина не указана.
     */
    void reject(const std::string& reason);

    /**
     * @brief Возвращает причину отказа.
     * @return Причина отказа.
     * @throws BusinessRuleException Если обращение не отклонено.
     */
    const std::string& rejectionReason() const;

private:
    int originalOrderNumber_;
    std::string failedUnit_;
    Date submittedOn_;
    bool resolved_;
    bool accepted_;
    std::string rejectionReason_;
};

}
