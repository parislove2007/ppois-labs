#pragma once

#include "operations/Operation.h"

namespace fs
{

/**
 * @brief Запись о create записи по пути.
 */
class CreateOperation : public Operation
{
public:
    CreateOperation(Path path, Timestamp at);

    std::string kind() const override;
    std::string describe() const override;

protected:
    Path path_;
};

}
