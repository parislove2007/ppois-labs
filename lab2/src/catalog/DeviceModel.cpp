#include "catalog/DeviceModel.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kEarliestReleaseYear = 1950;
}

namespace repair
{

DeviceModel::DeviceModel(const Manufacturer& manufacturer, const DeviceCategory& category,
                         const std::string& modelName, int releaseYear)
    : manufacturer_(manufacturer), category_(category),
      modelName_(modelName), releaseYear_(releaseYear)
{
    if (modelName.empty())
    {
        throw ValidationException("Обозначение модели обязательно");
    }
    if (releaseYear < kEarliestReleaseYear)
    {
        throw ValidationException("Неправдоподобный год выпуска: "
                                  + std::to_string(releaseYear));
    }
}

const Manufacturer& DeviceModel::manufacturer() const { return manufacturer_; }

const DeviceCategory& DeviceModel::category() const { return category_; }

const std::string& DeviceModel::modelName() const { return modelName_; }

int DeviceModel::releaseYear() const { return releaseYear_; }

std::string DeviceModel::fullName() const
{
    return manufacturer_.name() + " " + modelName_ + ", " + category_.name();
}

int DeviceModel::ageInYears(const Date& today) const
{
    const int age = today.year() - releaseYear_;
    return age < 0 ? 0 : age;
}

bool DeviceModel::isObsolete(const Date& today, int supportedYears) const
{
    return ageInYears(today) > supportedYears;
}

bool DeviceModel::operator==(const DeviceModel& other) const
{
    return manufacturer_ == other.manufacturer_ && modelName_ == other.modelName_;
}

std::ostream& operator<<(std::ostream& out, const DeviceModel& model)
{
    out << model.fullName();
    return out;
}

}
