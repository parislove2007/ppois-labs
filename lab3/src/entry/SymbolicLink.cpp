#include "entry/SymbolicLink.h"

namespace fs
{

SymbolicLink::SymbolicLink(std::string name, Path target, std::string owner,
                           Permission permission, Timestamp created)
    : FileSystemEntry(std::move(name), std::move(owner), permission, created),
      target_(std::move(target)),
      followCount_(0)
{
}

const Path& SymbolicLink::target() const
{
    return target_;
}

void SymbolicLink::retarget(Path newTarget, Timestamp at)
{
    target_ = std::move(newTarget);
    touch(at);
}

void SymbolicLink::registerFollow()
{
    ++followCount_;
}

int SymbolicLink::followCount() const
{
    return followCount_;
}

FileSize SymbolicLink::size() const
{
    return FileSize();
}

std::string SymbolicLink::describe() const
{
    return "[link] " + name_ + " -> " + target_.toString();
}

}
