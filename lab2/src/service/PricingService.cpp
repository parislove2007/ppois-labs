#include "service/PricingService.h"

namespace repair
{

PricingService::PricingService(const PriceList& priceList) : priceList_(priceList) {}

bool PricingService::isPriceKnown(const ServiceType& serviceType) const
{
    return priceList_.contains(serviceType);
}

Money PricingService::workCost(const ServiceType& serviceType, int repetitions) const
{
    return priceList_.totalFor(serviceType, repetitions);
}

Money PricingService::priceForClient(const Client& client, const Money& amount) const
{
    return client.loyaltyDiscount().remainderOf(amount);
}

Money PricingService::bestPriceFor(const Client& client, const Money& amount,
                                   const Discount& promotion) const
{
    const Money byLoyalty = priceForClient(client, amount);
    const Money byPromotion = promotion.applyTo(amount);
    return byPromotion < byLoyalty ? byPromotion : byLoyalty;
}

Money PricingService::orderTotal(const RepairOrder& order) const
{
    return priceForClient(order.client(), order.totalCost());
}

}
