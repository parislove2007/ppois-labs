#include "repair/Device.h"

#include <algorithm>

#include "error/ValidationException.h"

namespace
{
const int kNoDays = 0;
}

namespace repair
{

Device::Device(const DeviceModel& model, const SerialNumber& serialNumber,
               const Date& purchaseDate)
    : model_(model), serialNumber_(serialNumber), purchaseDate_(purchaseDate) {}

const DeviceModel& Device::model() const { return model_; }

const SerialNumber& Device::serialNumber() const { return serialNumber_; }

const Date& Device::purchaseDate() const { return purchaseDate_; }

std::size_t Device::accessoryCount() const { return accessories_.size(); }

void Device::addAccessory(const std::string& accessory)
{
    if (accessory.empty())
    {
        throw ValidationException("Название принадлежности не может быть пустым");
    }
    if (hasAccessory(accessory))
    {
        return;
    }
    accessories_.push_back(accessory);
}

bool Device::hasAccessory(const std::string& accessory) const
{
    return std::find(accessories_.begin(), accessories_.end(), accessory) != accessories_.end();
}

bool Device::isUnderFactoryWarranty(const Date& today, int factoryWarrantyMonths) const
{
    const Date warrantyEnd = purchaseDate_.plusMonths(factoryWarrantyMonths);
    return !today.isAfter(warrantyEnd);
}

int Device::ageInDays(const Date& today) const
{
    const int days = purchaseDate_.daysUntil(today);
    return days < kNoDays ? kNoDays : days;
}

bool Device::operator==(const Device& other) const
{
    return serialNumber_ == other.serialNumber_;
}

}
