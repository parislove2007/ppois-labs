#include "value/Clock.h"

namespace fs
{

Clock::Clock(std::int64_t startSeconds) : current_(startSeconds)
{
}

Timestamp Clock::next()
{
    return Timestamp::of(current_++);
}

Timestamp Clock::now() const
{
    return Timestamp::of(current_);
}

}
