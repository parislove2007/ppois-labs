#include "repair/RepairOrder.h"

#include "error/BusinessRuleException.h"
#include "error/InvalidStatusTransitionException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"

namespace
{
const int kMinOrderNumber = 1;
const std::string kSystemActor = "Система";
}

namespace repair
{

RepairOrder::RepairOrder(int number, const std::shared_ptr<Client>& client,
                         const IntakeAct& intakeAct, const Date& openedOn)
    : number_(number), client_(client), intakeAct_(intakeAct),
      status_(OrderStatus::Accepted), openedOn_(openedOn)
{
    if (number < kMinOrderNumber)
    {
        throw ValidationException("Номер заказа должен быть положительным");
    }
    if (!client)
    {
        throw ValidationException("Клиент заказа не задан");
    }
}

int RepairOrder::number() const { return number_; }

const Client& RepairOrder::client() const { return *client_; }

const IntakeAct& RepairOrder::intakeAct() const { return intakeAct_; }

OrderStatus RepairOrder::status() const { return status_; }

const Date& RepairOrder::openedOn() const { return openedOn_; }

const Estimate& RepairOrder::estimate() const { return estimate_; }

std::size_t RepairOrder::historySize() const { return history_.size(); }

const OrderHistoryEntry& RepairOrder::historyAt(std::size_t index) const
{
    if (index >= history_.size())
    {
        throw NotFoundException("Запись журнала с таким номером отсутствует");
    }
    return history_[index];
}

bool RepairOrder::isClosed() const
{
    return status_ == OrderStatus::Issued || status_ == OrderStatus::Cancelled;
}

bool RepairOrder::hasTechnician() const { return technician_ != nullptr; }

const Technician& RepairOrder::technician() const
{
    if (!hasTechnician())
    {
        throw NotFoundException("Мастер на заказ не назначен");
    }
    return *technician_;
}

std::string RepairOrder::actorName() const
{
    return hasTechnician() ? technician_->name().shortName() : kSystemActor;
}

bool RepairOrder::isTransitionAllowed(OrderStatus from, OrderStatus target)
{
    if (target == OrderStatus::Cancelled)
    {
        return from == OrderStatus::Accepted || from == OrderStatus::Diagnosed
               || from == OrderStatus::EstimateApproved;
    }
    switch (from)
    {
        case OrderStatus::Accepted: return target == OrderStatus::Diagnosed;
        case OrderStatus::Diagnosed: return target == OrderStatus::EstimateApproved;
        case OrderStatus::EstimateApproved: return target == OrderStatus::InRepair;
        case OrderStatus::InRepair: return target == OrderStatus::Completed;
        case OrderStatus::Completed: return target == OrderStatus::Issued;
        default: return false;
    }
}

void RepairOrder::changeStatus(OrderStatus target, const Date& changedOn,
                               const std::string& changedBy)
{
    if (!isTransitionAllowed(status_, target))
    {
        throw InvalidStatusTransitionException(toString(status_), toString(target));
    }
    history_.push_back(OrderHistoryEntry(status_, target, changedOn, changedBy));
    status_ = target;
}

void RepairOrder::assignTechnician(const std::shared_ptr<Technician>& technician)
{
    if (isClosed())
    {
        throw BusinessRuleException("Нельзя назначить мастера на закрытый заказ");
    }
    if (hasTechnician())
    {
        throw BusinessRuleException("Мастер на заказ уже назначен");
    }
    if (!technician->canServe(intakeAct_.device().model().category()))
    {
        throw BusinessRuleException("У мастера нет специализации по категории «"
                                    + intakeAct_.device().model().category().name() + "»");
    }
    technician->takeOrder();
    technician_ = technician;
}

void RepairOrder::recordDiagnostics(const Diagnostics& diagnostics, const Date& performedOn)
{
    changeStatus(OrderStatus::Diagnosed, performedOn, diagnostics.performedBy());
    diagnostics_ = diagnostics;
}

const Diagnostics& RepairOrder::diagnostics() const
{
    if (!diagnostics_.has_value())
    {
        throw NotFoundException("Диагностика по заказу не проводилась");
    }
    return diagnostics_.value();
}

void RepairOrder::addWorkToEstimate(const WorkItem& work)
{
    if (status_ != OrderStatus::Diagnosed)
    {
        throw InvalidStatusTransitionException(toString(status_),
                                               toString(OrderStatus::Diagnosed));
    }
    estimate_.addWork(work);
}

void RepairOrder::addPartToEstimate(const PartUsage& usage)
{
    if (status_ != OrderStatus::Diagnosed)
    {
        throw InvalidStatusTransitionException(toString(status_),
                                               toString(OrderStatus::Diagnosed));
    }
    estimate_.addPart(usage);
}

void RepairOrder::approveEstimate(const Date& approvedOn)
{
    changeStatus(OrderStatus::EstimateApproved, approvedOn, client_->name().shortName());
    estimate_.approve(approvedOn);
}

void RepairOrder::startRepair(const Date& startedOn)
{
    if (!hasTechnician())
    {
        throw BusinessRuleException("Нельзя начать ремонт без назначенного мастера");
    }
    changeStatus(OrderStatus::InRepair, startedOn, actorName());
}

void RepairOrder::completeRepair(const Date& completedOn, int warrantyMonths)
{
    changeStatus(OrderStatus::Completed, completedOn, actorName());
    Warranty issued(completedOn, warrantyMonths);
    if (diagnostics_.has_value())
    {
        for (std::size_t index = 0; index < diagnostics_->defectCount(); ++index)
        {
            issued.addCoveredUnit(diagnostics_->defectAt(index).unit());
        }
    }
    warranty_ = issued;
}

void RepairOrder::issueToClient(const Date& issuedOn)
{
    changeStatus(OrderStatus::Issued, issuedOn, actorName());
    client_->registerCompletedOrder();
    if (hasTechnician())
    {
        technician_->releaseOrder();
    }
}

void RepairOrder::cancel(const Date& cancelledOn, const std::string& reason)
{
    if (reason.empty())
    {
        throw ValidationException("Причина отмены заказа обязательна");
    }
    changeStatus(OrderStatus::Cancelled, cancelledOn, reason);
    if (hasTechnician())
    {
        technician_->releaseOrder();
    }
}

const Warranty& RepairOrder::warranty() const
{
    if (!warranty_.has_value())
    {
        throw NotFoundException("Гарантия не выдана: ремонт не завершён");
    }
    return warranty_.value();
}

Money RepairOrder::totalCost() const { return estimate_.totalCost(); }

}
