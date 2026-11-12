#pragma once

#include "catalog/PriceList.h"
#include "catalog/ServiceType.h"
#include "finance/Discount.h"
#include "people/Client.h"
#include "repair/RepairOrder.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Расчёт стоимости работ и заказов.
 *
 * Сервис: собственного изменяемого состояния не имеет, прайс-лист
 * задаётся при создании и дальше только читается. Логика скидок
 * собрана здесь, а не размазана по сущностям.
 */
class PricingService
{
public:
    /**
     * @brief Создаёт сервис, работающий по заданному прайсу.
     * @param priceList Действующий прайс-лист.
     */
    explicit PricingService(const PriceList& priceList);

    /**
     * @brief Проверяет, есть ли цена на вид работы.
     * @param serviceType Вид работы.
     * @return true, если цена задана в прайсе.
     */
    bool isPriceKnown(const ServiceType& serviceType) const;

    /**
     * @brief Считает стоимость работ по прайсу.
     * @param serviceType Вид работы.
     * @param repetitions Число повторений.
     * @return Стоимость работ.
     * @throws NotFoundException Если цена не задана.
     */
    Money workCost(const ServiceType& serviceType, int repetitions) const;

    /**
     * @brief Применяет к сумме накопительную скидку клиента.
     * @param client Клиент.
     * @param amount Исходная сумма.
     * @return Сумма к оплате.
     */
    Money priceForClient(const Client& client, const Money& amount) const;

    /**
     * @brief Выбирает выгодную клиенту из двух скидок и применяет её.
     * @param client Клиент.
     * @param amount Исходная сумма.
     * @param promotion Акционная скидка.
     * @return Сумма к оплате по лучшей из скидок.
     */
    Money bestPriceFor(const Client& client, const Money& amount,
                       const Discount& promotion) const;

    /**
     * @brief Считает итог по заказу с учётом скидки его клиента.
     * @param order Заказ-наряд.
     * @return Сумма к оплате.
     */
    Money orderTotal(const RepairOrder& order) const;

private:
    PriceList priceList_;
};

}
