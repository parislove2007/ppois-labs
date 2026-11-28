#pragma once

#include "operations/Operation.h"

namespace fs
{

/**
 * @brief Запись о move записи между путями.
 */
class MoveOperation : public Operation
{
public:
    MoveOperation(Path from, Path to, Timestamp at);

    std::string kind() const override;
    std::string describe() const override;

    const Path& from() const;
    const Path& to() const;

private:
    Path from_;
    Path to_;
};

}
