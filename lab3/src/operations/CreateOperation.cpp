#include "operations/CreateOperation.h"

namespace fs
{

CreateOperation::CreateOperation(Path path, Timestamp at) : Operation(at), path_(std::move(path))
{
}

std::string CreateOperation::kind() const
{
    return "create";
}

std::string CreateOperation::describe() const
{
    return "created " + path_.toString();
}

}
