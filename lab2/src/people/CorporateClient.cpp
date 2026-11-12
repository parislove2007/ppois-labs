#include "people/CorporateClient.h"

#include "error/ValidationException.h"

namespace
{
const std::string kClientKindName = "Организация по договору";
}

namespace repair
{

CorporateClient::CorporateClient(const PersonName& name, const PhoneNumber& phone,
                                 const EmailAddress& email, const Address& address,
                                 const std::string& companyName,
                                 const std::string& contractNumber,
                                 const Percentage& contractDiscount)
    : Client(name, phone, email, address), companyName_(companyName),
      contractNumber_(contractNumber), contractDiscount_(contractDiscount)
{
    if (companyName.empty() || contractNumber.empty())
    {
        throw ValidationException("Название организации и номер договора обязательны");
    }
}

std::string CorporateClient::clientKind() const { return kClientKindName; }

Percentage CorporateClient::loyaltyDiscount() const
{
    const Percentage accumulated = Client::loyaltyDiscount();
    return accumulated < contractDiscount_ ? contractDiscount_ : accumulated;
}

const std::string& CorporateClient::companyName() const { return companyName_; }

const std::string& CorporateClient::contractNumber() const { return contractNumber_; }

const Percentage& CorporateClient::contractDiscount() const { return contractDiscount_; }

}
