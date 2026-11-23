#include "user/RegularUser.h"

namespace fs
{

std::string RegularUser::role() const
{
    return "regular";
}

Permission RegularUser::defaultPermission() const
{
    return Permission::readWrite();
}


}
