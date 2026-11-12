#include "value/PersonName.h"

#include <ostream>

#include "error/ValidationException.h"

namespace
{
const std::string kInitialSuffix = ". ";
const unsigned char kAsciiLimit = 0x80;
const unsigned char kTwoByteMask = 0xE0;
const unsigned char kTwoByteLead = 0xC0;
const unsigned char kThreeByteMask = 0xF0;
const unsigned char kThreeByteLead = 0xE0;
const unsigned char kFourByteMask = 0xF8;
const unsigned char kFourByteLead = 0xF0;
const std::size_t kOneByte = 1;
const std::size_t kTwoBytes = 2;
const std::size_t kThreeBytes = 3;
const std::size_t kFourBytes = 4;

/**
 * Длина первого символа строки в байтах. Кодировка UTF-8 переменной
 * ширины: кириллическая буква занимает два байта, поэтому брать
 * ровно один байт нельзя — получится испорченная последовательность.
 */
std::size_t firstCharacterLength(const std::string& text)
{
    const unsigned char lead = static_cast<unsigned char>(text.front());
    if (lead < kAsciiLimit)
    {
        return kOneByte;
    }
    if ((lead & kTwoByteMask) == kTwoByteLead)
    {
        return kTwoBytes;
    }
    if ((lead & kThreeByteMask) == kThreeByteLead)
    {
        return kThreeBytes;
    }
    if ((lead & kFourByteMask) == kFourByteLead)
    {
        return kFourBytes;
    }
    return kOneByte;
}

std::string initialOf(const std::string& part)
{
    if (part.empty())
    {
        return "";
    }
    return part.substr(0, firstCharacterLength(part)) + kInitialSuffix;
}
}

namespace repair
{

PersonName::PersonName(const std::string& surname, const std::string& givenName)
    : PersonName(surname, givenName, "") {}

PersonName::PersonName(const std::string& surname, const std::string& givenName,
                       const std::string& patronymic)
    : surname_(surname), givenName_(givenName), patronymic_(patronymic)
{
    if (surname.empty() || givenName.empty())
    {
        throw ValidationException("Фамилия и имя обязательны для заполнения");
    }
}

const std::string& PersonName::surname() const { return surname_; }

const std::string& PersonName::givenName() const { return givenName_; }

const std::string& PersonName::patronymic() const { return patronymic_; }

bool PersonName::hasPatronymic() const { return !patronymic_.empty(); }

std::string PersonName::fullName() const
{
    if (!hasPatronymic())
    {
        return surname_ + " " + givenName_;
    }
    return surname_ + " " + givenName_ + " " + patronymic_;
}

std::string PersonName::shortName() const
{
    const std::string initials = initialOf(givenName_) + initialOf(patronymic_);
    return surname_ + " " + initials;
}

bool PersonName::operator==(const PersonName& other) const
{
    return surname_ == other.surname_
           && givenName_ == other.givenName_
           && patronymic_ == other.patronymic_;
}

std::ostream& operator<<(std::ostream& out, const PersonName& name)
{
    out << name.fullName();
    return out;
}

}
