#include "repair/OrderStatus.h"

namespace repair
{

std::string toString(OrderStatus status)
{
    switch (status)
    {
        case OrderStatus::Accepted: return "Принят";
        case OrderStatus::Diagnosed: return "Продиагностирован";
        case OrderStatus::EstimateApproved: return "Смета утверждена";
        case OrderStatus::InRepair: return "В ремонте";
        case OrderStatus::Completed: return "Готов к выдаче";
        case OrderStatus::Issued: return "Выдан";
        case OrderStatus::Cancelled: return "Отменён";
    }
    return "Неизвестное состояние";
}

}
