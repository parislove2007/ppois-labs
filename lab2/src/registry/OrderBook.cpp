#include "registry/OrderBook.h"

#include <iterator>
#include <algorithm>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/OrderNotFoundException.h"
#include "error/ValidationException.h"

namespace
{
const int kFirstOrderNumber = 1;
}

namespace repair
{

OrderBook::OrderBook() : nextNumber_(kFirstOrderNumber) {}

std::size_t OrderBook::size() const { return orders_.size(); }

int OrderBook::nextNumber() const { return nextNumber_; }

int OrderBook::issueNumber()
{
    const int issued = nextNumber_;
    ++nextNumber_;
    return issued;
}

void OrderBook::add(const std::shared_ptr<RepairOrder>& order)
{
    if (!order)
    {
        throw ValidationException("Регистрируемый заказ не задан");
    }
    if (contains(order->number()))
    {
        throw BusinessRuleException("Заказ с номером " + std::to_string(order->number())
                                    + " уже есть в журнале");
    }
    orders_.push_back(order);
}

bool OrderBook::contains(int number) const
{
    return std::any_of(orders_.begin(), orders_.end(),
                       [number](const std::shared_ptr<RepairOrder>& order)
                       { return order->number() == number; });
}

std::shared_ptr<RepairOrder> OrderBook::findByNumber(int number) const
{
    const auto position = std::find_if(orders_.begin(), orders_.end(),
                                       [number](const std::shared_ptr<RepairOrder>& order)
                                       { return order->number() == number; });
    if (position == orders_.end())
    {
        throw OrderNotFoundException(number);
    }
    return *position;
}

std::shared_ptr<RepairOrder> OrderBook::orderAt(std::size_t index) const
{
    if (index >= orders_.size())
    {
        throw NotFoundException("Заказ с таким номером отсутствует в журнале");
    }
    return orders_[index];
}

std::vector<std::shared_ptr<RepairOrder>> OrderBook::ordersWithStatus(OrderStatus status) const
{
    std::vector<std::shared_ptr<RepairOrder>> selected;
    std::copy_if(orders_.begin(), orders_.end(), std::back_inserter(selected),
                 [status](const std::shared_ptr<RepairOrder>& order)
                 { return order->status() == status; });
    return selected;
}

std::size_t OrderBook::openOrderCount() const
{
    return static_cast<std::size_t>(
        std::count_if(orders_.begin(), orders_.end(),
                      [](const std::shared_ptr<RepairOrder>& order)
                      { return !order->isClosed(); }));
}

}
