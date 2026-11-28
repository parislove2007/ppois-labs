#include "operations/DeleteOperation.h"

namespace fs
{

DeleteOperation::DeleteOperation(Path path, Timestamp at) : Operation(at), path_(std::move(path))
{
}

std::string DeleteOperation::kind() const
{
    return "delete";
}

std::string DeleteOperation::describe() const
{
    return "deleted " + path_.toString();
}

}
