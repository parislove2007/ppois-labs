#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "catalog/DeviceCategory.h"
#include "catalog/DeviceModel.h"
#include "catalog/Manufacturer.h"
#include "catalog/Part.h"
#include "catalog/PriceList.h"
#include "catalog/PriceListEntry.h"
#include "catalog/ServiceType.h"
#include "error/NotFoundException.h"
#include "error/ValidationException.h"

using namespace repair;

namespace
{
const int kWashingMachineWarrantyMonths = 6;
const int kSampleReleaseYear = 2020;

Manufacturer sampleManufacturer() { return Manufacturer("Bosch", "Германия"); }

DeviceCategory sampleCategory()
{
    return DeviceCategory("Стиральные машины", kWashingMachineWarrantyMonths);
}

DeviceModel sampleModel()
{
    return DeviceModel(sampleManufacturer(), sampleCategory(), "WAN2427", kSampleReleaseYear);
}

ServiceType sampleService()
{
    return ServiceType("Замена подшипника", Duration::fromHours(2, 0), true);
}

template <typename Value>
std::string printed(const Value& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}
}

TEST(ManufacturerTest, StoresNameAndCountry)
{
    const Manufacturer maker = sampleManufacturer();

    EXPECT_EQ(maker.name(), "Bosch");
    EXPECT_EQ(maker.country(), "Германия");
    EXPECT_EQ(printed(maker), "Bosch (Германия)");
}

TEST(ManufacturerTest, DetectsDomesticOrigin)
{
    const Manufacturer maker = sampleManufacturer();

    EXPECT_TRUE(maker.isDomesticFor("Германия"));
    EXPECT_FALSE(maker.isDomesticFor("Беларусь"));
}

TEST(ManufacturerTest, RequiresNameAndCountry)
{
    EXPECT_THROW(Manufacturer("", "Германия"), ValidationException);
    EXPECT_THROW(Manufacturer("Bosch", ""), ValidationException);
    EXPECT_TRUE(sampleManufacturer() == Manufacturer("Bosch", "Германия"));
}

TEST(DeviceCategoryTest, StoresNameAndWarranty)
{
    const DeviceCategory category = sampleCategory();

    EXPECT_EQ(category.name(), "Стиральные машины");
    EXPECT_EQ(category.baseWarrantyMonths(), kWashingMachineWarrantyMonths);
    EXPECT_EQ(printed(category), "Стиральные машины");
}

TEST(DeviceCategoryTest, MatchesNameIgnoringCase)
{
    EXPECT_TRUE(DeviceCategory("Laptops", 12).matchesName("laptops"));
    EXPECT_FALSE(DeviceCategory("Laptops", 12).matchesName("Fridges"));
}

TEST(DeviceCategoryTest, ScalesWarrantyByComplexityUpToCap)
{
    const DeviceCategory category = sampleCategory();

    EXPECT_EQ(category.warrantyMonthsFor(2), 12);
    EXPECT_EQ(category.warrantyMonthsFor(100), 36);
    EXPECT_THROW(category.warrantyMonthsFor(0), ValidationException);
}

TEST(DeviceCategoryTest, RejectsEmptyNameAndNegativeWarranty)
{
    EXPECT_THROW(DeviceCategory("", 6), ValidationException);
    EXPECT_THROW(DeviceCategory("Фены", -1), ValidationException);
    EXPECT_TRUE(sampleCategory() == sampleCategory());
}

TEST(DeviceModelTest, BuildsFullName)
{
    EXPECT_EQ(sampleModel().fullName(), "Bosch WAN2427, Стиральные машины");
    EXPECT_EQ(printed(sampleModel()), "Bosch WAN2427, Стиральные машины");
    EXPECT_EQ(sampleModel().modelName(), "WAN2427");
    EXPECT_EQ(sampleModel().releaseYear(), kSampleReleaseYear);
    EXPECT_TRUE(sampleModel().manufacturer() == sampleManufacturer());
    EXPECT_TRUE(sampleModel().category() == sampleCategory());
}

TEST(DeviceModelTest, ComputesAgeAndObsolescence)
{
    const DeviceModel model = sampleModel();
    const Date today(2026, 1, 1);

    EXPECT_EQ(model.ageInYears(today), 6);
    EXPECT_EQ(model.ageInYears(Date(2015, 1, 1)), 0);
    EXPECT_TRUE(model.isObsolete(today, 5));
    EXPECT_FALSE(model.isObsolete(today, 10));
}

TEST(DeviceModelTest, RejectsEmptyNameAndImplausibleYear)
{
    EXPECT_THROW(DeviceModel(sampleManufacturer(), sampleCategory(), "", 2020),
                 ValidationException);
    EXPECT_THROW(DeviceModel(sampleManufacturer(), sampleCategory(), "X", 1900),
                 ValidationException);
    EXPECT_TRUE(sampleModel() == sampleModel());
}

TEST(PartTest, StoresArticleNameAndPrice)
{
    const Part part("BRG-6204", "Подшипник 6204", Money::fromRubles(25, 0));

    EXPECT_EQ(part.article(), "BRG-6204");
    EXPECT_EQ(part.name(), "Подшипник 6204");
    EXPECT_EQ(part.price().rubles(), 25);
    EXPECT_EQ(printed(part), "BRG-6204 — Подшипник 6204");
}

TEST(PartTest, TracksCompatibilityWithoutDuplicates)
{
    Part part("BRG-6204", "Подшипник", Money(100));

    EXPECT_FALSE(part.isCompatibleWith(sampleModel()));
    part.declareCompatibleWith(sampleModel());
    part.declareCompatibleWith(sampleModel());

    EXPECT_TRUE(part.isCompatibleWith(sampleModel()));
    EXPECT_EQ(part.compatibleModelCount(), 1u);
}

TEST(PartTest, CountsBatchPrice)
{
    const Part part("BRG-6204", "Подшипник", Money::fromRubles(25, 0));

    EXPECT_EQ(part.totalPriceFor(4).rubles(), 100);
    EXPECT_THROW(part.totalPriceFor(0), ValidationException);
}

TEST(PartTest, RejectsEmptyArticleOrName)
{
    EXPECT_THROW(Part("", "Подшипник", Money(1)), ValidationException);
    EXPECT_THROW(Part("BRG", "", Money(1)), ValidationException);
    EXPECT_TRUE(Part("A1", "X", Money(1)) == Part("A1", "Y", Money(2)));
}

TEST(ServiceTypeTest, StoresNameDurationAndFlag)
{
    const ServiceType service = sampleService();

    EXPECT_EQ(service.name(), "Замена подшипника");
    EXPECT_EQ(service.normativeDuration().totalMinutes(), 120);
    EXPECT_TRUE(service.requiresDiagnostics());
    EXPECT_TRUE(service.matchesName("Замена подшипника"));
    EXPECT_FALSE(service.matchesName("Чистка"));
    EXPECT_EQ(printed(service), "Замена подшипника");
}

TEST(ServiceTypeTest, ScalesDurationByRepetitions)
{
    EXPECT_EQ(sampleService().estimatedDurationFor(3).totalMinutes(), 360);
    EXPECT_THROW(sampleService().estimatedDurationFor(0), ValidationException);
}

TEST(ServiceTypeTest, RejectsEmptyName)
{
    EXPECT_THROW(ServiceType("", Duration(10), false), ValidationException);
    EXPECT_TRUE(sampleService() == sampleService());
}

TEST(PriceListEntryTest, CountsTotalForRepetitions)
{
    const PriceListEntry entry(sampleService(), Money::fromRubles(40, 0));

    EXPECT_EQ(entry.totalFor(3).rubles(), 120);
    EXPECT_TRUE(entry.describes(sampleService()));
    EXPECT_FALSE(entry.describes(ServiceType("Чистка", Duration(10), false)));
    EXPECT_EQ(entry.price().rubles(), 40);
    EXPECT_TRUE(entry.serviceType() == sampleService());
    EXPECT_THROW(entry.totalFor(0), ValidationException);
}

TEST(PriceListEntryTest, PrintsServiceAndPrice)
{
    const PriceListEntry entry(ServiceType("Чистка", Duration(30), false), Money::fromRubles(10, 0));

    EXPECT_EQ(printed(entry), "Чистка — 10.00 руб.");
}

TEST(PriceListTest, StartsEmpty)
{
    const PriceList prices;

    EXPECT_TRUE(prices.isEmpty());
    EXPECT_EQ(prices.size(), 0u);
}

TEST(PriceListTest, StoresAndReplacesPrices)
{
    PriceList prices;
    prices.setPrice(sampleService(), Money::fromRubles(40, 0));
    prices.setPrice(sampleService(), Money::fromRubles(50, 0));

    EXPECT_EQ(prices.size(), 1u);
    EXPECT_TRUE(prices.contains(sampleService()));
    EXPECT_EQ(prices.priceFor(sampleService()).rubles(), 50);
    EXPECT_EQ(prices.entryAt(0).price().rubles(), 50);
}

TEST(PriceListTest, CountsTotalForSeveralRepetitions)
{
    PriceList prices;
    prices.setPrice(sampleService(), Money::fromRubles(40, 0));

    EXPECT_EQ(prices.totalFor(sampleService(), 2).rubles(), 80);
    EXPECT_THROW(prices.totalFor(sampleService(), 0), ValidationException);
}

TEST(PriceListTest, ReportsMissingServiceAndIndex)
{
    PriceList prices;

    EXPECT_FALSE(prices.contains(sampleService()));
    EXPECT_THROW(prices.priceFor(sampleService()), NotFoundException);
    EXPECT_THROW(prices.entryAt(0), NotFoundException);
}
