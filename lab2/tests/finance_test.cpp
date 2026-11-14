#include <gtest/gtest.h>

#include <string>

#include "error/BusinessRuleException.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"
#include "finance/Discount.h"
#include "finance/Invoice.h"
#include "finance/InvoiceLine.h"
#include "finance/Payment.h"

using namespace repair;

namespace
{
const int kInvoiceNumber = 500;
const int kOrderNumber = 101;

Discount volumeDiscount()
{
    return Discount(Percentage(10), "Скидка за объём", Money::fromRubles(100, 0));
}

Payment cashPayment(long long rubles)
{
    return Payment(Money::fromRubles(rubles, 0), PaymentMethod::Cash, Date(2026, 1, 15),
                   "ЧЕК-1");
}

Invoice invoiceWithLines()
{
    Invoice invoice(kInvoiceNumber, kOrderNumber, Date(2026, 1, 14));
    invoice.addLine(InvoiceLine("Замена подшипника", Money::fromRubles(60, 0)));
    invoice.addLine(InvoiceLine("Запчасти", Money::fromRubles(50, 0)));
    return invoice;
}
}

TEST(DiscountTest, AppliesOnlyAboveThreshold)
{
    const Discount discount = volumeDiscount();

    EXPECT_TRUE(discount.appliesTo(Money::fromRubles(100, 0)));
    EXPECT_FALSE(discount.appliesTo(Money::fromRubles(99, 0)));
    EXPECT_EQ(discount.amountFor(Money::fromRubles(200, 0)).rubles(), 20);
    EXPECT_TRUE(discount.amountFor(Money::fromRubles(50, 0)).isZero());
    EXPECT_EQ(discount.applyTo(Money::fromRubles(200, 0)).rubles(), 180);
}

TEST(DiscountTest, ComparesByBenefitOnGivenAmount)
{
    const Discount small(Percentage(5), "Постоянный клиент", Money());
    const Discount large = volumeDiscount();

    EXPECT_TRUE(large.isBetterThan(small, Money::fromRubles(200, 0)));
    EXPECT_FALSE(large.isBetterThan(small, Money::fromRubles(50, 0)));
}

TEST(DiscountTest, ExposesFieldsAndValidatesReason)
{
    const Discount discount = volumeDiscount();

    EXPECT_EQ(discount.percentage().value(), 10);
    EXPECT_EQ(discount.reason(), "Скидка за объём");
    EXPECT_EQ(discount.minimumAmount().rubles(), 100);
    EXPECT_THROW(Discount(Percentage(5), "", Money()), ValidationException);
}

TEST(PaymentTest, TranslatesEveryMethod)
{
    EXPECT_EQ(toString(PaymentMethod::Cash), "Наличные");
    EXPECT_EQ(toString(PaymentMethod::Card), "Банковская карта");
    EXPECT_EQ(toString(PaymentMethod::Transfer), "Безналичный перевод");
}

TEST(PaymentTest, DistinguishesCashlessAndTimeliness)
{
    const Payment cash = cashPayment(50);
    const Payment card(Money::fromRubles(50, 0), PaymentMethod::Card, Date(2026, 1, 20),
                       "ТРМ-9");

    EXPECT_FALSE(cash.isCashless());
    EXPECT_TRUE(card.isCashless());
    EXPECT_TRUE(cash.isOnTime(Date(2026, 1, 15)));
    EXPECT_FALSE(card.isOnTime(Date(2026, 1, 15)));
    EXPECT_EQ(cash.method(), PaymentMethod::Cash);
    EXPECT_EQ(cash.reference(), "ЧЕК-1");
    EXPECT_TRUE(cash.receivedOn() == Date(2026, 1, 15));
    EXPECT_EQ(cash.amount().rubles(), 50);
    EXPECT_EQ(cash.describe(), "15.01.2026, Наличные, 50.00, документ ЧЕК-1");
}

TEST(PaymentTest, RejectsZeroAmountAndMissingReference)
{
    EXPECT_THROW(Payment(Money(), PaymentMethod::Cash, Date(2026, 1, 15), "ЧЕК"),
                 ValidationException);
    EXPECT_THROW(Payment(Money(100), PaymentMethod::Cash, Date(2026, 1, 15), ""),
                 ValidationException);
}

TEST(InvoiceLineTest, StoresDescriptionAndAmount)
{
    const InvoiceLine line("Диагностика", Money::fromRubles(20, 0));

    EXPECT_EQ(line.description(), "Диагностика");
    EXPECT_EQ(line.amount().rubles(), 20);
    EXPECT_TRUE(line.exceeds(Money::fromRubles(19, 0)));
    EXPECT_FALSE(line.exceeds(Money::fromRubles(20, 0)));
    EXPECT_EQ(line.describe(), "Диагностика — 20.00");
}

TEST(InvoiceLineTest, RejectsEmptyDescriptionAndZeroAmount)
{
    EXPECT_THROW(InvoiceLine("", Money(100)), ValidationException);
    EXPECT_THROW(InvoiceLine("Диагностика", Money()), ValidationException);
}

TEST(InvoiceTest, SumsLinesAndTracksBalance)
{
    const Invoice invoice = invoiceWithLines();

    EXPECT_EQ(invoice.lineCount(), 2u);
    EXPECT_EQ(invoice.subtotal().rubles(), 110);
    EXPECT_TRUE(invoice.discountAmount().isZero());
    EXPECT_EQ(invoice.total().rubles(), 110);
    EXPECT_EQ(invoice.balanceDue().rubles(), 110);
    EXPECT_FALSE(invoice.isPaid());
    EXPECT_EQ(invoice.number(), kInvoiceNumber);
    EXPECT_EQ(invoice.orderNumber(), kOrderNumber);
    EXPECT_TRUE(invoice.issuedOn() == Date(2026, 1, 14));
    EXPECT_EQ(invoice.lineAt(0).description(), "Замена подшипника");
}

TEST(InvoiceTest, AppliesDiscountToSubtotal)
{
    Invoice invoice = invoiceWithLines();
    invoice.applyDiscount(volumeDiscount());

    EXPECT_EQ(invoice.discountAmount().rubles(), 11);
    EXPECT_EQ(invoice.total().rubles(), 99);
}

TEST(InvoiceTest, AcceptsPartialPaymentsUntilClosed)
{
    Invoice invoice = invoiceWithLines();
    invoice.registerPayment(cashPayment(60));

    EXPECT_EQ(invoice.paidAmount().rubles(), 60);
    EXPECT_EQ(invoice.balanceDue().rubles(), 50);
    EXPECT_FALSE(invoice.isPaid());

    invoice.registerPayment(cashPayment(50));

    EXPECT_TRUE(invoice.isPaid());
    EXPECT_EQ(invoice.paymentCount(), 2u);
    EXPECT_EQ(invoice.paymentAt(1).amount().rubles(), 50);
}

TEST(InvoiceTest, RefusesOverpaymentAndPaymentAfterClosing)
{
    Invoice invoice = invoiceWithLines();

    EXPECT_THROW(invoice.registerPayment(cashPayment(200)), BusinessRuleException);

    invoice.registerPayment(cashPayment(110));
    EXPECT_THROW(invoice.registerPayment(cashPayment(1)), BusinessRuleException);
}

TEST(InvoiceTest, FreezesContentAfterFirstPayment)
{
    Invoice invoice = invoiceWithLines();
    invoice.registerPayment(cashPayment(10));

    EXPECT_THROW(invoice.addLine(InvoiceLine("Ещё", Money(100))), BusinessRuleException);
    EXPECT_THROW(invoice.applyDiscount(volumeDiscount()), BusinessRuleException);
}

TEST(InvoiceTest, ReportsBadIndexAndValidatesNumbers)
{
    const Invoice invoice(kInvoiceNumber, kOrderNumber, Date(2026, 1, 14));

    EXPECT_THROW(invoice.lineAt(0), NotFoundException);
    EXPECT_THROW(invoice.paymentAt(0), NotFoundException);
    EXPECT_THROW(Invoice(0, kOrderNumber, Date(2026, 1, 14)), ValidationException);
    EXPECT_THROW(Invoice(kInvoiceNumber, 0, Date(2026, 1, 14)), ValidationException);
}
