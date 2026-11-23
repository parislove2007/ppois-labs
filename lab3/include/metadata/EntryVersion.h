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
    EntryVersion(int number, std::string content, Timestamp saved);

    int number() const;
    const std::string& content() const;
    Timestamp saved() const;

    /** @return Контрольная сумма содержимого версии. */
    Checksum checksum() const;

private:
    int number_;
    std::string content_;
    Timestamp saved_;
};

}
