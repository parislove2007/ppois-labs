#include "operations/RenameOperation.h"

namespace fs
{

RenameOperation::RenameOperation(Path path, std::string newName, Timestamp at)
    : Operation(at), path_(std::move(path)), newName_(std::move(newName))
{
}

std::string RenameOperation::kind() const
{
    return "rename";
}

std::string RenameOperation::describe() const
{
    return "renamed " + path_.toString() + " to " + newName_;
}

const std::string& RenameOperation::newName() const
{
    return newName_;
}

}
