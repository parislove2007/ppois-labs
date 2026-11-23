#include "value/Clock.h"

namespace fs
{

Clock::Clock(std::int64_t startSeconds, std::int64_t stepSeconds)
    : current_(startSeconds), stepSeconds_(stepSeconds)
{
}

Timestamp Clock::next()
{
    const Timestamp now = Timestamp::of(current_);
    current_ += stepSeconds_;
    return now;
}

Timestamp Clock::now() const
{
    return Timestamp::of(current_);
}

}
