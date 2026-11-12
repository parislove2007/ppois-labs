#include "service/WarrantyService.h"

namespace
{
const std::string kNotCoveredReason = "Узел не покрыт гарантией или срок гарантии истёк";
}

namespace repair
{

WarrantyService::WarrantyService() {}

int WarrantyService::warrantyMonthsFor(const RepairOrder& order) const
{
    const DeviceCategory& category = order.intakeAct().device().model().category();
    return category.warrantyMonthsFor(order.diagnostics().complexityFactor());
}

bool WarrantyService::isCoveredCase(const RepairOrder& order, const WarrantyClaim& claim) const
{
    return claim.matches(order.warranty());
}

void WarrantyService::process(const RepairOrder& order, WarrantyClaim& claim) const
{
    if (isCoveredCase(order, claim))
    {
        claim.accept();
        return;
    }
    claim.reject(kNotCoveredReason);
}

Date WarrantyService::coverageEnd(const RepairOrder& order) const
{
    return order.warranty().expiryDate();
}

}
