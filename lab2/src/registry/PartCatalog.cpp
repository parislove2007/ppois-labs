#include "registry/PartCatalog.h"

#include <algorithm>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/PartNotFoundException.h"

namespace repair
{

PartCatalog::PartCatalog() {}

std::size_t PartCatalog::size() const { return parts_.size(); }

void PartCatalog::add(const Part& part)
{
    if (contains(part.article()))
    {
        throw BusinessRuleException("Запчасть с артикулом " + part.article()
                                    + " уже есть в каталоге");
    }
    parts_.push_back(part);
}

bool PartCatalog::contains(const std::string& article) const
{
    return std::any_of(parts_.begin(), parts_.end(),
                       [&article](const Part& part) { return part.article() == article; });
}

const Part& PartCatalog::findByArticle(const std::string& article) const
{
    const auto position = std::find_if(parts_.begin(), parts_.end(),
                                       [&article](const Part& part)
                                       { return part.article() == article; });
    if (position == parts_.end())
    {
        throw PartNotFoundException(article);
    }
    return *position;
}

const Part& PartCatalog::partAt(std::size_t index) const
{
    if (index >= parts_.size())
    {
        throw NotFoundException("Запчасть с таким номером отсутствует в каталоге");
    }
    return parts_[index];
}

std::vector<Part> PartCatalog::partsFor(const DeviceModel& model) const
{
    std::vector<Part> compatible;
    for (const Part& part : parts_)
    {
        if (part.isCompatibleWith(model))
        {
            compatible.push_back(part);
        }
    }
    return compatible;
}

void PartCatalog::declareCompatibility(const std::string& article, const DeviceModel& model)
{
    const auto position = std::find_if(parts_.begin(), parts_.end(),
                                       [&article](const Part& part)
                                       { return part.article() == article; });
    if (position == parts_.end())
    {
        throw PartNotFoundException(article);
    }
    position->declareCompatibleWith(model);
}

}
