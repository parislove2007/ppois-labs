#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "error/InvalidMoneyAmountException.h"
#include "error/InvalidPhoneNumberException.h"
#include "error/InvalidSerialNumberException.h"
#include "error/ValidationException.h"
#include "value/Address.h"
#include "value/Date.h"
#include "value/Duration.h"
#include "value/EmailAddress.h"
#include "value/Money.h"
#include "value/Percentage.h"
#include "value/PersonName.h"
#include "value/PhoneNumber.h"
#include "value/SerialNumber.h"

using namespace repair;

namespace
{
const std::string kSamplePhone = "+375 (29) 123-45-67";
const std::string kSampleSerial = "WM-2024-0001";

template <typename Value>
std::string printed(const Value& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}
}

TEST(MoneyTest, DefaultAmountIsZero)
{
    EXPECT_TRUE(Money().isZero());
}

TEST(MoneyTest, NegativeAmountThrows)
{
    EXPECT_THROW(Money(-1), InvalidMoneyAmountException);
}

TEST(MoneyTest, SplitsIntoRublesAndKopecks)
{
    const Money amount = Money::fromRubles(123, 45);

    EXPECT_EQ(amount.kopecks(), 12345);
    EXPECT_EQ(amount.rubles(), 123);
    EXPECT_EQ(amount.kopecksPart(), 45);
}

TEST(MoneyTest, RejectsKopecksPartOutsideRange)
{
    EXPECT_THROW(Money::fromRubles(1, 100), InvalidMoneyAmountException);
    EXPECT_THROW(Money::fromRubles(-1, 0), InvalidMoneyAmountException);
}

TEST(MoneyTest, AddsAndMultiplies)
{
    const Money first = Money::fromRubles(10, 50);
    const Money second = Money::fromRubles(4, 50);

    EXPECT_EQ((first + second).kopecks(), 1500);
    EXPECT_EQ((second * 2).kopecks(), 900);
}

TEST(MoneyTest, SubtractionBelowZeroThrows)
{
    EXPECT_THROW(Money(100) - Money(200), InvalidMoneyAmountException);
}

TEST(MoneyTest, NegativeMultiplierThrows)
{
    EXPECT_THROW(Money(100) * -1, InvalidMoneyAmountException);
}

TEST(MoneyTest, SubtractsWithinRange)
{
    EXPECT_EQ((Money(500) - Money(200)).kopecks(), 300);
}

TEST(MoneyTest, ComparesByAmount)
{
    EXPECT_TRUE(Money(100) < Money(200));
    EXPECT_TRUE(Money(100) == Money(100));
}

TEST(MoneyTest, PadsKopecksInText)
{
    EXPECT_EQ(Money::fromRubles(7, 5).toString(), "7.05");
    EXPECT_EQ(printed(Money::fromRubles(7, 5)), "7.05 руб.");
}

TEST(PercentageTest, DefaultShareIsZero)
{
    EXPECT_TRUE(Percentage().isZero());
}

TEST(PercentageTest, RejectsValuesOutsideRange)
{
    EXPECT_THROW(Percentage(-1), ValidationException);
    EXPECT_THROW(Percentage(101), ValidationException);
}

TEST(PercentageTest, TakesShareOfAmount)
{
    const Percentage discount(10);

    EXPECT_EQ(discount.applyTo(Money(1000)).kopecks(), 100);
    EXPECT_EQ(discount.remainderOf(Money(1000)).kopecks(), 900);
    EXPECT_EQ(discount.value(), 10);
}

TEST(PercentageTest, CombinationIsCappedAtHundred)
{
    const Percentage combined = Percentage(70).combinedWith(Percentage(50));

    EXPECT_EQ(combined.value(), 100);
    EXPECT_EQ(Percentage(10).combinedWith(Percentage(5)).value(), 15);
}

TEST(PercentageTest, ComparesAndPrints)
{
    EXPECT_TRUE(Percentage(5) < Percentage(10));
    EXPECT_TRUE(Percentage(5) == Percentage(5));
    EXPECT_EQ(printed(Percentage(15)), "15%");
}

TEST(DurationTest, DefaultDurationIsZero)
{
    EXPECT_TRUE(Duration().isZero());
}

TEST(DurationTest, NegativeDurationThrows)
{
    EXPECT_THROW(Duration(-5), ValidationException);
    EXPECT_THROW(Duration::fromHours(-1, 0), ValidationException);
}

TEST(DurationTest, SplitsIntoHoursAndMinutes)
{
    const Duration work = Duration::fromHours(2, 30);

    EXPECT_EQ(work.totalMinutes(), 150);
    EXPECT_EQ(work.hours(), 2);
    EXPECT_EQ(work.minutesPart(), 30);
    EXPECT_EQ(work.toString(), "2 ч 30 мин");
}

TEST(DurationTest, AddsAndCompares)
{
    const Duration first(90);
    const Duration second(30);

    EXPECT_EQ((first + second).totalMinutes(), 120);
    EXPECT_TRUE(first.isLongerThan(second));
    EXPECT_TRUE(first == Duration(90));
    EXPECT_EQ(printed(second), "0 ч 30 мин");
}

TEST(PhoneNumberTest, KeepsOnlyDigits)
{
    const PhoneNumber number(kSamplePhone);

    EXPECT_EQ(number.digits(), "375291234567");
    EXPECT_EQ(number.operatorCode(), "29");
}

TEST(PhoneNumberTest, DifferentWritingsAreEqual)
{
    EXPECT_TRUE(PhoneNumber(kSamplePhone) == PhoneNumber("375291234567"));
}

TEST(PhoneNumberTest, RejectsWrongLengthAndCountryCode)
{
    EXPECT_FALSE(PhoneNumber::isValid("12345"));
    EXPECT_FALSE(PhoneNumber::isValid("445291234567"));
    EXPECT_THROW(PhoneNumber("12345"), InvalidPhoneNumberException);
}

TEST(PhoneNumberTest, FormatsForDisplay)
{
    EXPECT_EQ(printed(PhoneNumber("375291234567")), kSamplePhone);
}

TEST(EmailAddressTest, SplitsIntoParts)
{
    const EmailAddress address("client@example.com");

    EXPECT_EQ(address.localPart(), "client");
    EXPECT_EQ(address.domain(), "example.com");
    EXPECT_TRUE(address.belongsToDomain("example.com"));
    EXPECT_EQ(address.value(), "client@example.com");
}

TEST(EmailAddressTest, RejectsMalformedAddresses)
{
    EXPECT_FALSE(EmailAddress::isValid("no-at-sign"));
    EXPECT_FALSE(EmailAddress::isValid("two@at@signs.com"));
    EXPECT_FALSE(EmailAddress::isValid("@nolocal.com"));
    EXPECT_FALSE(EmailAddress::isValid("nodomain@"));
    EXPECT_FALSE(EmailAddress::isValid("nodot@example"));
    EXPECT_THROW(EmailAddress("broken"), ValidationException);
}

TEST(EmailAddressTest, ComparesAndPrints)
{
    EXPECT_TRUE(EmailAddress("a@b.by") == EmailAddress("a@b.by"));
    EXPECT_EQ(printed(EmailAddress("a@b.by")), "a@b.by");
}

TEST(SerialNumberTest, NormalisesCase)
{
    EXPECT_TRUE(SerialNumber("wm-2024-0001") == SerialNumber(kSampleSerial));
    EXPECT_EQ(SerialNumber("wm-2024-0001").value(), kSampleSerial);
}

TEST(SerialNumberTest, RejectsBadLengthAndSymbols)
{
    EXPECT_FALSE(SerialNumber::isValid("AB"));
    EXPECT_FALSE(SerialNumber::isValid("AB CD"));
    EXPECT_FALSE(SerialNumber::isValid(std::string(25, 'A')));
    EXPECT_THROW(SerialNumber("A"), InvalidSerialNumberException);
}

TEST(SerialNumberTest, ExtractsBatchPrefix)
{
    EXPECT_EQ(SerialNumber(kSampleSerial).batchPrefix(2), "WM");
    EXPECT_THROW(SerialNumber(kSampleSerial).batchPrefix(100), ValidationException);
}

TEST(SerialNumberTest, OrdersAndPrints)
{
    EXPECT_TRUE(SerialNumber("AAAA") < SerialNumber("BBBB"));
    EXPECT_EQ(printed(SerialNumber(kSampleSerial)), kSampleSerial);
}

TEST(PersonNameTest, BuildsFullAndShortForms)
{
    const PersonName name("Иванов", "Пётр", "Сергеевич");

    EXPECT_EQ(name.fullName(), "Иванов Пётр Сергеевич");
    EXPECT_TRUE(name.hasPatronymic());
    EXPECT_EQ(name.surname(), "Иванов");
    EXPECT_EQ(name.givenName(), "Пётр");
    EXPECT_EQ(name.patronymic(), "Сергеевич");
}

TEST(PersonNameTest, WorksWithoutPatronymic)
{
    const PersonName name("Smith", "John");

    EXPECT_FALSE(name.hasPatronymic());
    EXPECT_EQ(name.fullName(), "Smith John");
    EXPECT_EQ(name.shortName(), "Smith J. ");
    EXPECT_EQ(PersonName("Иванов", "Пётр", "Сергеевич").shortName(), "Иванов П. С. ");
}

TEST(PersonNameTest, RequiresSurnameAndGivenName)
{
    EXPECT_THROW(PersonName("", "Пётр"), ValidationException);
    EXPECT_THROW(PersonName("Иванов", ""), ValidationException);
}

TEST(PersonNameTest, ComparesAndPrints)
{
    EXPECT_TRUE(PersonName("Smith", "John") == PersonName("Smith", "John"));
    EXPECT_EQ(printed(PersonName("Smith", "John")), "Smith John");
}

TEST(AddressTest, DistinguishesHouseFromApartment)
{
    const Address privateHouse("Минск", "Лесная", "7");
    const Address flat("Минск", "Лесная", "7", "12");

    EXPECT_TRUE(privateHouse.isPrivateHouse());
    EXPECT_FALSE(flat.isPrivateHouse());
    EXPECT_EQ(privateHouse.oneLine(), "Минск, ул. Лесная, д. 7");
    EXPECT_EQ(flat.oneLine(), "Минск, ул. Лесная, д. 7, кв. 12");
}

TEST(AddressTest, ComparesCityAndWholeAddress)
{
    const Address first("Минск", "Лесная", "7");
    const Address second("Минск", "Полевая", "3");

    EXPECT_TRUE(first.isSameCity(second));
    EXPECT_FALSE(first == second);
    EXPECT_EQ(first.city(), "Минск");
    EXPECT_EQ(first.street(), "Лесная");
    EXPECT_EQ(first.house(), "7");
    EXPECT_EQ(first.apartment(), "");
}

TEST(AddressTest, RequiresMandatoryParts)
{
    EXPECT_THROW(Address("", "Лесная", "7"), ValidationException);
    EXPECT_THROW(Address("Минск", "", "7"), ValidationException);
    EXPECT_THROW(Address("Минск", "Лесная", ""), ValidationException);
}

TEST(AddressTest, PrintsOneLine)
{
    EXPECT_EQ(printed(Address("Минск", "Лесная", "7")), "Минск, ул. Лесная, д. 7");
}

TEST(DateTest, RejectsNonExistentDates)
{
    EXPECT_FALSE(Date::isValid(2025, 2, 29));
    EXPECT_FALSE(Date::isValid(2025, 13, 1));
    EXPECT_FALSE(Date::isValid(2025, 1, 0));
    EXPECT_THROW(Date(2025, 2, 30), ValidationException);
}

TEST(DateTest, AcceptsLeapDay)
{
    EXPECT_TRUE(Date::isValid(2024, 2, 29));
    EXPECT_EQ(Date(2024, 2, 29).day(), 29);
}

TEST(DateTest, CountsDaysBetweenDates)
{
    const Date start(2025, 1, 1);

    EXPECT_EQ(start.daysUntil(Date(2025, 1, 31)), 30);
    EXPECT_EQ(Date(2025, 1, 31).daysUntil(start), -30);
}

TEST(DateTest, ShiftsByDaysAcrossMonthBoundary)
{
    const Date shifted = Date(2025, 1, 31).plusDays(1);

    EXPECT_EQ(shifted.year(), 2025);
    EXPECT_EQ(shifted.month(), 2);
    EXPECT_EQ(shifted.day(), 1);
}

TEST(DateTest, ShiftsByMonthsClampingToLastDay)
{
    const Date clamped = Date(2025, 1, 31).plusMonths(1);

    EXPECT_EQ(clamped.month(), 2);
    EXPECT_EQ(clamped.day(), 28);
    EXPECT_EQ(Date(2025, 1, 15).plusMonths(2).month(), 3);
}

TEST(DateTest, OrdersDates)
{
    const Date earlier(2025, 1, 1);
    const Date later(2025, 6, 1);

    EXPECT_TRUE(earlier.isBefore(later));
    EXPECT_TRUE(later.isAfter(earlier));
    EXPECT_TRUE(earlier < later);
    EXPECT_TRUE(earlier == Date(2025, 1, 1));
}

TEST(DateTest, PrintsWithLeadingZeros)
{
    EXPECT_EQ(Date(2025, 3, 7).toString(), "07.03.2025");
    EXPECT_EQ(printed(Date(2025, 12, 25)), "25.12.2025");
}
