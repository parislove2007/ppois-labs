#pragma once

#include "user/User.h"

namespace fs
{

/**
 * @brief Роль «administrator».
 */
class Administrator : public User
{
public:
    using User::User;

    std::string role() const override;
    Permission defaultPermission() const override;
    bool canBypassQuota() const override;
};

}
