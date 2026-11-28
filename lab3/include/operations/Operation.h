#pragma once

#include <string>

#include "value/Path.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Запись журнала о выполненной операции.
 *
 * Абстрактный класс: каждая операция знает свой вид и собирает
 * человекочитаемое описание из своих данных.
 */
class Operation
{
public:
    /**
     * @brief Создаёт запись об операции.
     * @param at Момент выполнения.
     */
    explicit Operation(Timestamp at);

    virtual ~Operation() = default;

    /** @return Момент выполнения. */
    Timestamp at() const;

    /** @return Вид операции: «create», «delete», «move», «copy», «rename». */
    virtual std::string kind() const = 0;

    /** @return Человекочитаемое описание операции. */
    virtual std::string describe() const = 0;

protected:
    Timestamp at_;
};

}
