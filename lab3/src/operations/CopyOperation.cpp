#include "operations/CopyOperation.h"

namespace fs
{

CopyOperation::CopyOperation(Path from, Path to, Timestamp at)
    : Operation(at), from_(std::move(from)), to_(std::move(to))
{
}

std::string CopyOperation::kind() const
{
    return "copy";
}

std::string CopyOperation::describe() const
{
    return "copied " + from_.toString() + " -> " + to_.toString();
}

const Path& CopyOperation::from() const
{
    return from_;
}

const Path& CopyOperation::to() const
{
    return to_;
}

}
