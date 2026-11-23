#include "user/Administrator.h"

namespace fs
{

std::string Administrator::role() const
{
    return "administrator";
}

Permission Administrator::defaultPermission() const
{
    return Permission::full();
}

bool Administrator::canBypassQuota() const
{
    return true;
}

}
