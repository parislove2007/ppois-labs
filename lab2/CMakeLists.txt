cmake_minimum_required(VERSION 3.20)
project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_library(lab2_core STATIC
    src/catalog/DeviceCategory.cpp
    src/catalog/DeviceModel.cpp
    src/catalog/Manufacturer.cpp
    src/catalog/Part.cpp
    src/catalog/PriceList.cpp
    src/catalog/PriceListEntry.cpp
    src/catalog/ServiceType.cpp
    src/error/BusinessRuleException.cpp
    src/error/ClientNotFoundException.cpp
    src/error/EstimateNotApprovedException.cpp
    src/error/InvalidMoneyAmountException.cpp
    src/error/InvalidPhoneNumberException.cpp
    src/error/InvalidSerialNumberException.cpp
    src/error/InvalidStatusTransitionException.cpp
    src/error/NotFoundException.cpp
    src/error/OrderNotFoundException.cpp
    src/error/OutOfStockException.cpp
    src/error/PartNotFoundException.cpp
    src/error/RepairException.cpp
    src/error/ValidationException.cpp
    src/error/WarrantyExpiredException.cpp
    src/finance/Discount.cpp
    src/finance/Invoice.cpp
    src/finance/InvoiceLine.cpp
    src/finance/Payment.cpp
    src/people/Client.cpp
    src/people/CorporateClient.cpp
    src/people/Employee.cpp
    src/people/Manager.cpp
    src/people/Technician.cpp
    src/registry/ClientRegistry.cpp
    src/registry/EmployeeRegistry.cpp
    src/registry/OrderBook.cpp
    src/registry/PartCatalog.cpp
    src/repair/Defect.cpp
    src/repair/Device.cpp
    src/repair/Diagnostics.cpp
    src/repair/Estimate.cpp
    src/repair/IntakeAct.cpp
    src/repair/OrderHistoryEntry.cpp
    src/repair/OrderStatus.cpp
    src/repair/PartUsage.cpp
    src/repair/RepairOrder.cpp
    src/repair/Warranty.cpp
    src/repair/WarrantyClaim.cpp
    src/repair/WorkItem.cpp
    src/schedule/Appointment.cpp
    src/schedule/TimeSlot.cpp
    src/schedule/WorkSchedule.cpp
    src/service/InventoryService.cpp
    src/service/OrderService.cpp
    src/service/PricingService.cpp
    src/service/SchedulingService.cpp
    src/service/WarrantyService.cpp
    src/stock/PartReservation.cpp
    src/stock/StockItem.cpp
    src/stock/Supplier.cpp
    src/stock/SupplyOrder.cpp
    src/stock/SupplyOrderItem.cpp
    src/stock/Warehouse.cpp
    src/value/Address.cpp
    src/value/Date.cpp
    src/value/Duration.cpp
    src/value/EmailAddress.cpp
    src/value/Money.cpp
    src/value/Percentage.cpp
    src/value/PersonName.cpp
    src/value/PhoneNumber.cpp
    src/value/SerialNumber.cpp
)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(lab2_core PUBLIC --coverage -O0 -g)
    target_link_options(lab2_core PUBLIC --coverage)
endif()

target_include_directories(lab2_core PUBLIC include)

add_executable(lab2_app
    app/ConsoleMenu.cpp
    app/main.cpp
)
target_include_directories(lab2_app PRIVATE app)
target_link_libraries(lab2_app PRIVATE lab2_core)

add_executable(lab2_tests
    tests/catalog_test.cpp
    tests/error_test.cpp
    tests/finance_test.cpp
    tests/people_test.cpp
    tests/registry_test.cpp
    tests/repair_test.cpp
    tests/schedule_test.cpp
    tests/service_test.cpp
    tests/stock_test.cpp
    tests/value_test.cpp
)
target_link_libraries(lab2_tests PRIVATE lab2_core GTest::gtest_main)
gtest_discover_tests(lab2_tests)
