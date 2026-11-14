#include "error/InvalidMoneyAmountException.h"

namespace
{
std::string buildMessage(long long kopecks)
{
    return "Недопустимая денежная сумма: " + std::to_string(kopecks) + " коп.";
}
}

namespace repair
{

InvalidMoneyAmountException::InvalidMoneyAmountException(long long kopecks)
    : ValidationException(buildMessage(kopecks)),
      kopecks_(kopecks) {}

long long InvalidMoneyAmountException::kopecks() const { return kopecks_; }

}
