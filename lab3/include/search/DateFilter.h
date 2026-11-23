#pragma once

#include "search/SearchFilter.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Критерий «запись менялась в диапазоне дат».
 */
class DateFilter : public SearchFilter
{
public:
    DateFilter(Timestamp from, Timestamp to);

    bool matches(const Path& path, const FileSystemEntry& entry) const override;

private:
    Timestamp from_;
    Timestamp to_;
};

}
