#include "stock/StockItem.h"

#include "error/BusinessRuleException.h"
#include "error/OutOfStockException.h"
#include "error/ValidationException.h"

namespace
{
const int kNothing = 0;
const int kMinAmount = 1;
}

namespace repair
{

StockItem::StockItem(const Part& part, int quantity, const std::string& location)
    : part_(part), quantity_(quantity), reserved_(kNothing), location_(location)
{
    if (quantity < kNothing)
    {
        throw ValidationException("Остаток на складе не может быть отрицательным");
    }
    if (location.empty())
    {
        throw ValidationException("Место хранения запчасти не указано");
    }
}

const Part& StockItem::part() const { return part_; }

int StockItem::quantity() const { return quantity_; }

int StockItem::reservedQuantity() const { return reserved_; }

const std::string& StockItem::location() const { return location_; }

int StockItem::availableQuantity() const { return quantity_ - reserved_; }

void StockItem::receive(int amount)
{
    if (amount < kMinAmount)
    {
        throw ValidationException("Приходуемое количество должно быть положительным");
    }
    quantity_ += amount;
}

void StockItem::reserve(int amount)
{
    if (amount < kMinAmount)
    {
        throw ValidationException("Резервируемое количество должно быть положительным");
    }
    if (amount > availableQuantity())
    {
        throw OutOfStockException(part_.article(), amount, availableQuantity());
    }
    reserved_ += amount;
}

void StockItem::releaseReservation(int amount)
{
    if (amount < kMinAmount || amount > reserved_)
    {
        throw BusinessRuleException("Нельзя снять резерв: зарезервировано "
                                    + std::to_string(reserved_) + " шт.");
    }
    reserved_ -= amount;
}

void StockItem::writeOff(int amount)
{
    if (amount < kMinAmount || amount > reserved_)
    {
        throw BusinessRuleException("Списывать можно только зарезервированные запчасти");
    }
    reserved_ -= amount;
    quantity_ -= amount;
}

bool StockItem::isBelowMinimum(int minimum) const { return quantity_ < minimum; }

}
