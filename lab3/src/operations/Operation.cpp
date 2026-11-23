#include "operations/Operation.h"

namespace fs
{

Operation::Operation(Timestamp at) : at_(at)
{
}

Timestamp Operation::at() const
{
    return at_;
}

}
