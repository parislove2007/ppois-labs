#pragma once

#include "user/User.h"

namespace fs
{

/**
 * @brief Роль «regular».
 */
class RegularUser : public User
{
public:
    using User::User;

    std::string role() const override;
    Permission defaultPermission() const override;

};

}
