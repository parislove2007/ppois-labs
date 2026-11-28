#pragma once

#include <iosfwd>
#include <string>

#include "value/Duration.h"

namespace repair
{

/**
 * @brief Вид выполняемой работы: диагностика, замена узла, чистка.
 *
 * Справочная запись. Нормативная длительность используется при
 * планировании загрузки мастера, а признак обязательной диагностики —
 * при проверке последовательности работ по заказу.
 */
class ServiceType
{
public:
    /**
     * @brief Создаёт вид работы.
     * @param name Название работы.
     * @param normativeDuration Нормативная длительность одной операции.
     * @param requiresDiagnostics Требуется ли предварительная диагностика.
     * @throws ValidationException Если название пусто.
     */
    ServiceType(const std::string& name, const Duration& normativeDuration,
                bool requiresDiagnostics);

    /** @return Название работы. */
    const std::string& name() const;

    /** @return Нормативная длительность одной операции. */
    const Duration& normativeDuration() const;

    /** @return true, если работа требует предварительной диагностики. */
    bool requiresDiagnostics() const;

    /**
     * @brief Считает нормативное время на несколько повторений работы.
     * @param repetitions Число повторений.
     * @return Суммарная длительность.
     * @throws ValidationException Если число повторений не положительно.
     */
    Duration estimatedDurationFor(int repetitions) const;

    /**
     * @brief Сравнивает название работы с заданным.
     * @param candidate Проверяемое название.
     * @return true, если названия совпадают.
     */
    bool matchesName(const std::string& candidate) const;

    /** @brief Сравнивает виды работ по названию. */
    bool operator==(const ServiceType& other) const;

private:
    std::string name_;
    Duration normativeDuration_;
    bool requiresDiagnostics_;
};

/** @brief Записывает вид работы в поток. */
std::ostream& operator<<(std::ostream& out, const ServiceType& serviceType);

}
