#pragma once

#include "operations/Operation.h"

namespace fs
{

/**
 * @brief Запись о delete записи по пути.
 */
class DeleteOperation : public Operation
{
public:
    DeleteOperation(Path path, Timestamp at);

    std::string kind() const override;
    std::string describe() const override;

protected:
    Path path_;
};

}
