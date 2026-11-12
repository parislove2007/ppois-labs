#pragma once

#include <string>

#include "value/Date.h"
#include "value/PhoneNumber.h"

namespace repair
{

/**
 * @brief Поставщик запчастей.
 *
 * Рейтинг накапливается по итогам поставок и влияет на выбор
 * поставщика при дозаказе: из двух с одинаковым сроком выбирается
 * более надёжный.
 */
class Supplier
{
public:
    /**
     * @brief Создаёт поставщика.
     * @param name Название поставщика.
     * @param phone Контактный телефон.
     * @param deliveryDays Типовой срок поставки в днях.
     * @param rating Начальный рейтинг, от одного до пяти.
     * @throws ValidationException Если название пусто, срок отрицателен
     *         или рейтинг вне диапазона.
     */
    Supplier(const std::string& name, const PhoneNumber& phone, int deliveryDays, int rating);

    /** @return Название поставщика. */
    const std::string& name() const;

    /** @return Контактный телефон. */
    const PhoneNumber& phone() const;

    /** @return Типовой срок поставки в днях. */
    int deliveryDays() const;

    /** @return Текущий рейтинг. */
    int rating() const;

    /**
     * @brief Считает ожидаемую дату поставки.
     * @param orderedOn Дата заказа.
     * @return Дата, к которой поставка должна прийти.
     */
    Date expectedDeliveryDate(const Date& orderedOn) const;

    /**
     * @brief Проверяет надёжность поставщика.
     * @param minimumRating Минимально приемлемый рейтинг.
     * @return true, если рейтинг не ниже требуемого.
     */
    bool isReliable(int minimumRating) const;

    /**
     * @brief Повышает рейтинг за поставку в срок.
     * @throws BusinessRuleException Если рейтинг уже максимальный.
     */
    void raiseRating();

    /**
     * @brief Понижает рейтинг за срыв поставки.
     * @throws BusinessRuleException Если рейтинг уже минимальный.
     */
    void lowerRating();

    /**
     * @brief Сравнивает поставщиков по предпочтительности.
     * @param other Поставщик для сравнения.
     * @return true, если текущий поставщик предпочтительнее.
     */
    bool isPreferableTo(const Supplier& other) const;

private:
    std::string name_;
    PhoneNumber phone_;
    int deliveryDays_;
    int rating_;
};

}
