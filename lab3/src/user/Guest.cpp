#include "user/Guest.h"

namespace fs
{

std::string Guest::role() const
{
    return "guest";
}

Permission Guest::defaultPermission() const
{
    return Permission::readOnly();
}


}
