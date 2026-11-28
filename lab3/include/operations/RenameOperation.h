#pragma once

#include <string>

#include "operations/Operation.h"
#include "value/Path.h"

namespace fs
{

/**
 * @brief Запись о переименовании записи.
 */
class RenameOperation : public Operation
{
public:
    RenameOperation(Path path, std::string newName, Timestamp at);

    std::string kind() const override;
    std::string describe() const override;

    /** @return Новое имя, которое получила запись. */
    const std::string& newName() const;

private:
    Path path_;
    std::string newName_;
};

}
