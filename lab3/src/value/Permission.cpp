#include "value/Permission.h"

#include <ostream>

namespace fs
{

Permission::Permission() : Permission(false, false, false)
{
}

Permission::Permission(bool read, bool write, bool execute)
    : read_(read), write_(write), execute_(execute)
{
}

Permission Permission::none()
{
    return Permission();
}

Permission Permission::readOnly()
{
    return Permission(true, false, false);
}

Permission Permission::readWrite()
{
    return Permission(true, true, false);
}

Permission Permission::full()
{
    return Permission(true, true, true);
}

bool Permission::canRead() const
{
    return read_;
}

bool Permission::canWrite() const
{
    return write_;
}

bool Permission::canExecute() const
{
    return execute_;
}

Permission Permission::withRead() const
{
    return Permission(true, write_, execute_);
}

Permission Permission::withWrite() const
{
    return Permission(read_, true, execute_);
}

Permission Permission::withExecute() const
{
    return Permission(read_, write_, true);
}

bool Permission::allows(const Permission& required) const
{
    return (!required.read_ || read_) && (!required.write_ || write_)
        && (!required.execute_ || execute_);
}

std::string Permission::toString() const
{
    std::string result = "---";
    if (read_) result[0] = 'r';
    if (write_) result[1] = 'w';
    if (execute_) result[2] = 'x';
    return result;
}

bool Permission::operator==(const Permission& other) const
{
    return read_ == other.read_ && write_ == other.write_ && execute_ == other.execute_;
}

std::ostream& operator<<(std::ostream& out, const Permission& permission)
{
    return out << permission.toString();
}

}
