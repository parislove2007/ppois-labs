#pragma once

#include <memory>
#include <string>
#include <vector>

#include "operations/Operation.h"

namespace fs
{

/**
 * @brief Журнал аудита: накапливает записи о всех операциях.
 */
class OperationJournal
{
public:
    /** @brief Добавляет запись; журнал забирает владение. */
    void log(std::unique_ptr<Operation> operation);

    /** @return Количество записей. */
    std::size_t count() const;

    /**
     * @brief Возвращает запись по индексу.
     * @throws PathNotFoundException Если индекс вне диапазона.
     */
    const Operation& at(std::size_t index) const;

    /** @return Указатели на записи заданного вида. */
    std::vector<const Operation*> byKind(const std::string& kind) const;

    /** @return Весь журнал одной строкой, по записи на строку. */
    std::string historyText() const;

private:
    std::vector<std::unique_ptr<Operation>> entries_;
};

}
