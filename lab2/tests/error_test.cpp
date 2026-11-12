#include <gtest/gtest.h>

#include <string>

#include "error/BusinessRuleException.h"
#include "error/ClientNotFoundException.h"
#include "error/EstimateNotApprovedException.h"
#include "error/InvalidMoneyAmountException.h"
#include "error/InvalidPhoneNumberException.h"
#include "error/InvalidSerialNumberException.h"
#include "error/InvalidStatusTransitionException.h"
#include "error/NotFoundException.h"
#include "error/OrderNotFoundException.h"
#include "error/OutOfStockException.h"
#include "error/PartNotFoundException.h"
#include "error/RepairException.h"
#include "error/ValidationException.h"
#include "error/WarrantyExpiredException.h"

using namespace repair;

namespace
{
const int kSampleOrderNumber = 1024;
}

TEST(ExceptionTest, BaseCarriesMessage)
{
    const RepairException error("сбой");

    EXPECT_STREQ(error.what(), "сбой");
}

TEST(ExceptionTest, ValidationBranchIsCaughtAsBase)
{
    EXPECT_THROW(throw ValidationException("плохие данные"), RepairException);
}

TEST(ExceptionTest, NotFoundBranchIsCaughtAsBase)
{
    EXPECT_THROW(throw NotFoundException("нет объекта"), RepairException);
}

TEST(ExceptionTest, BusinessRuleBranchIsCaughtAsBase)
{
    EXPECT_THROW(throw BusinessRuleException("нельзя"), RepairException);
}

TEST(ExceptionTest, InvalidMoneyAmountKeepsAmount)
{
    const InvalidMoneyAmountException error(-500);

    EXPECT_EQ(error.kopecks(), -500);
    EXPECT_NE(std::string(error.what()).find("-500"), std::string::npos);
}

TEST(ExceptionTest, InvalidPhoneNumberKeepsRawValue)
{
    const InvalidPhoneNumberException error("abc");

    EXPECT_EQ(error.rawNumber(), "abc");
}

TEST(ExceptionTest, InvalidSerialNumberKeepsRawValue)
{
    const InvalidSerialNumberException error("??");

    EXPECT_EQ(error.rawSerial(), "??");
}

TEST(ExceptionTest, ClientNotFoundKeepsPhone)
{
    const ClientNotFoundException error("+375291234567");

    EXPECT_EQ(error.phone(), "+375291234567");
    EXPECT_THROW(throw error, NotFoundException);
}

TEST(ExceptionTest, OrderNotFoundKeepsNumber)
{
    const OrderNotFoundException error(kSampleOrderNumber);

    EXPECT_EQ(error.orderNumber(), kSampleOrderNumber);
}

TEST(ExceptionTest, PartNotFoundKeepsArticle)
{
    const PartNotFoundException error("DRUM-77");

    EXPECT_EQ(error.article(), "DRUM-77");
}

TEST(ExceptionTest, InvalidStatusTransitionKeepsBothStatuses)
{
    const InvalidStatusTransitionException error("Принят", "Выдан");

    EXPECT_EQ(error.fromStatus(), "Принят");
    EXPECT_EQ(error.toStatus(), "Выдан");
    EXPECT_THROW(throw error, BusinessRuleException);
}

TEST(ExceptionTest, OutOfStockKeepsQuantities)
{
    const OutOfStockException error("BELT-5", 3, 1);

    EXPECT_EQ(error.article(), "BELT-5");
    EXPECT_EQ(error.requested(), 3);
    EXPECT_EQ(error.available(), 1);
}

TEST(ExceptionTest, WarrantyExpiredKeepsOrderAndDate)
{
    const WarrantyExpiredException error(kSampleOrderNumber, "01.01.2026");

    EXPECT_EQ(error.orderNumber(), kSampleOrderNumber);
    EXPECT_EQ(error.expiryDate(), "01.01.2026");
}

TEST(ExceptionTest, EstimateNotApprovedKeepsOrderNumber)
{
    const EstimateNotApprovedException error(kSampleOrderNumber);

    EXPECT_EQ(error.orderNumber(), kSampleOrderNumber);
}
