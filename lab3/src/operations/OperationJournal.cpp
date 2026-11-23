#include "operations/OperationJournal.h"

#include "error/PathNotFoundException.h"

namespace fs
{

void OperationJournal::log(std::unique_ptr<Operation> operation)
{
    entries_.push_back(std::move(operation));
}

std::size_t OperationJournal::count() const
{
    return entries_.size();
}

const Operation& OperationJournal::at(std::size_t index) const
{
    if (index >= entries_.size())
    {
        throw PathNotFoundException("No journal entry #" + std::to_string(index));
    }
    return *entries_[index];
}

std::vector<const Operation*> OperationJournal::byKind(const std::string& kind) const
{
    std::vector<const Operation*> result;
    for (const std::unique_ptr<Operation>& entry : entries_)
    {
        if (entry->kind() == kind)
        {
            result.push_back(entry.get());
        }
    }
    return result;
}

std::string OperationJournal::historyText() const
{
    std::string text;
    for (const std::unique_ptr<Operation>& entry : entries_)
    {
        text += "[" + entry->kind() + "] " + entry->describe() + "\n";
    }
    return text;
}

}
