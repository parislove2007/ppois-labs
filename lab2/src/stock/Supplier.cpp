#include "stock/Supplier.h"

#include "error/BusinessRuleException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinRating = 1;
const int kMaxRating = 5;
const int kMinDeliveryDays = 0;
}

namespace repair
{

Supplier::Supplier(const std::string& name, const PhoneNumber& phone, int deliveryDays,
                   int rating)
    : name_(name), phone_(phone), deliveryDays_(deliveryDays), rating_(rating)
{
    if (name.empty())
    {
        throw ValidationException("Название поставщика обязательно");
    }
    if (deliveryDays < kMinDeliveryDays)
    {
        throw ValidationException("Срок поставки не может быть отрицательным");
    }
    if (rating < kMinRating || rating > kMaxRating)
    {
        throw ValidationException("Рейтинг поставщика вне диапазона 1..5: "
                                  + std::to_string(rating));
    }
}

const std::string& Supplier::name() const { return name_; }

const PhoneNumber& Supplier::phone() const { return phone_; }

int Supplier::deliveryDays() const { return deliveryDays_; }

int Supplier::rating() const { return rating_; }

Date Supplier::expectedDeliveryDate(const Date& orderedOn) const
{
    return orderedOn.plusDays(deliveryDays_);
}

bool Supplier::isReliable(int minimumRating) const { return rating_ >= minimumRating; }

void Supplier::raiseRating()
{
    if (rating_ == kMaxRating)
    {
        throw BusinessRuleException("Рейтинг поставщика уже максимальный");
    }
    ++rating_;
}

void Supplier::lowerRating()
{
    if (rating_ == kMinRating)
    {
        throw BusinessRuleException("Рейтинг поставщика уже минимальный");
    }
    --rating_;
}

bool Supplier::isPreferableTo(const Supplier& other) const
{
    if (deliveryDays_ != other.deliveryDays_)
    {
        return deliveryDays_ < other.deliveryDays_;
    }
    return rating_ > other.rating_;
}

}
