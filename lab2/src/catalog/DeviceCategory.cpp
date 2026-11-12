#include "catalog/DeviceCategory.h"

#include <algorithm>
#include <cctype>
#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kMinComplexityFactor = 1;
const int kMaxWarrantyMonths = 36;

std::string toLowerCase(const std::string& text)
{
    std::string lower;
    for (const char symbol : text)
    {
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
    }
    return lower;
}
}

namespace repair
{

DeviceCategory::DeviceCategory(const std::string& name, int baseWarrantyMonths)
    : name_(name), baseWarrantyMonths_(baseWarrantyMonths)
{
    if (name.empty())
    {
        throw ValidationException("Название категории техники обязательно");
    }
    if (baseWarrantyMonths < 0)
    {
        throw ValidationException("Срок гарантии не может быть отрицательным");
    }
}

const std::string& DeviceCategory::name() const { return name_; }

int DeviceCategory::baseWarrantyMonths() const { return baseWarrantyMonths_; }

bool DeviceCategory::matchesName(const std::string& candidate) const
{
    return toLowerCase(name_) == toLowerCase(candidate);
}

int DeviceCategory::warrantyMonthsFor(int complexityFactor) const
{
    if (complexityFactor < kMinComplexityFactor)
    {
        throw ValidationException("Множитель сложности не может быть меньше единицы");
    }
    return std::min(baseWarrantyMonths_ * complexityFactor, kMaxWarrantyMonths);
}

bool DeviceCategory::operator==(const DeviceCategory& other) const { return name_ == other.name_; }

std::ostream& operator<<(std::ostream& out, const DeviceCategory& category)
{
    out << category.name();
    return out;
}

}
