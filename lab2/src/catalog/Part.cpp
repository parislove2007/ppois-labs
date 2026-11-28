#include "catalog/Part.h"

#include <algorithm>
#include <ostream>

#include "error/ValidationException.h"

namespace
{
const int kMinQuantity = 1;
}

namespace repair
{

Part::Part(const std::string& article, const std::string& name, const Money& price)
    : article_(article), name_(name), price_(price)
{
    if (article.empty() || name.empty())
    {
        throw ValidationException("Артикул и название запчасти обязательны");
    }
}

const std::string& Part::article() const { return article_; }

const std::string& Part::name() const { return name_; }

const Money& Part::price() const { return price_; }

std::size_t Part::compatibleModelCount() const { return compatibleModels_.size(); }

void Part::declareCompatibleWith(const DeviceModel& model)
{
    if (isCompatibleWith(model))
    {
        return;
    }
    compatibleModels_.push_back(model);
}

bool Part::isCompatibleWith(const DeviceModel& model) const
{
    return std::find(compatibleModels_.begin(), compatibleModels_.end(), model)
           != compatibleModels_.end();
}

Money Part::totalPriceFor(int quantity) const
{
    if (quantity < kMinQuantity)
    {
        throw ValidationException("Количество запчастей должно быть положительным");
    }
    return price_ * quantity;
}

bool Part::operator==(const Part& other) const { return article_ == other.article_; }

std::ostream& operator<<(std::ostream& out, const Part& part)
{
    out << part.article() << " — " << part.name();
    return out;
}

}
