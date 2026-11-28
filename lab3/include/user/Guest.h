#pragma once

#include "user/User.h"

namespace fs
{

/**
 * @brief Роль «guest».
 */
class Guest : public User
{
public:
    using User::User;

    std::string role() const override;
    Permission defaultPermission() const override;

};

}
