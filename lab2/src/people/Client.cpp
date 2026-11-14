#include "people/Client.h"

namespace
{
const int kNoOrders = 0;
const int kOrdersPerDiscountStep = 5;
const int kPercentPerStep = 2;
const int kMaxLoyaltyPercent = 10;
const std::string kClientKindName = "Частный клиент";
}

namespace repair
{

Client::Client(const PersonName& name, const PhoneNumber& phone, const EmailAddress& email,
               const Address& address)
    : name_(name), phone_(phone), email_(email), address_(address),
      completedOrders_(kNoOrders) {}

Client::~Client() {}

const PersonName& Client::name() const { return name_; }

const PhoneNumber& Client::phone() const { return phone_; }

const EmailAddress& Client::email() const { return email_; }

const Address& Client::address() const { return address_; }

int Client::completedOrders() const { return completedOrders_; }

std::string Client::clientKind() const { return kClientKindName; }

Percentage Client::loyaltyDiscount() const
{
    const int steps = completedOrders_ / kOrdersPerDiscountStep;
    const int earned = steps * kPercentPerStep;
    return Percentage(earned > kMaxLoyaltyPercent ? kMaxLoyaltyPercent : earned);
}

void Client::registerCompletedOrder() { ++completedOrders_; }

bool Client::isRegular(int requiredOrders) const { return completedOrders_ >= requiredOrders; }

std::string Client::contactLine() const
{
    return name_.shortName() + "— " + phone_.formatted();
}

}
