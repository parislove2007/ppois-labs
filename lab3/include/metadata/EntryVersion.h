#pragma once

#include <string>

#include "value/Checksum.h"
#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Снимок содержимого файла в момент сохранения версии.
 */
class EntryVersion
{
public:
    EntryVersion(int number, std::string content, Timestamp saved,
                 std::string author = "", std::string comment = "");

    int number() const;
    const std::string& content() const;
    Timestamp saved() const;
    const std::string& author() const;
    const std::string& comment() const;

    /** @return Контрольная сумма содержимого версии. */
    Checksum checksum() const;

private:
    int number_;
    std::string content_;
    Timestamp saved_;
    std::string author_;
    std::string comment_;
};

}
