#include "operations/MoveOperation.h"

namespace fs
{

MoveOperation::MoveOperation(Path from, Path to, Timestamp at)
    : Operation(at), from_(std::move(from)), to_(std::move(to))
{
}

std::string MoveOperation::kind() const
{
    return "move";
}

std::string MoveOperation::describe() const
{
    return "moved " + from_.toString() + " -> " + to_.toString();
}

const Path& MoveOperation::from() const
{
    return from_;
}

const Path& MoveOperation::to() const
{
    return to_;
}

}
