#include "storage/Quota.h"

namespace fs
{

Quota::Quota(std::string login, FileSize limit)
    : login_(std::move(login)), limit_(limit)
{
}

const std::string& Quota::login() const
{
    return login_;
}

FileSize Quota::limit() const
{
    return limit_;
}

FileSize Quota::used() const
{
    return used_;
}

FileSize Quota::remaining() const
{
    return limit_.fitsIn(used_) ? FileSize() : limit_ - used_;
}

void Quota::charge(FileSize delta)
{
    used_ = used_ + delta;
}

void Quota::release(FileSize delta)
{
    used_ = delta.fitsIn(used_) ? used_ - delta : FileSize();
}

}
