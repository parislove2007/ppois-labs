#pragma once

#include <string>

#include "value/FileSize.h"

namespace fs
{

/**
 * @brief Квота одного пользователя: лимит и занятые байты.
 */
class Quota
{
public:
    Quota(std::string login, FileSize limit);

    const std::string& login() const;
    FileSize limit() const;
    FileSize used() const;
    FileSize remaining() const;

    /** @brief Начисляет занятые байты. */
    void charge(FileSize delta);

    /** @brief Списывает освобождённые байты, не уходя ниже нуля. */
    void release(FileSize delta);

private:
    std::string login_;
    FileSize limit_;
    FileSize used_;
};

}
