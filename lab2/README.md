# Lab 2 — Household appliance repair service center

Object model of the "repair company" domain in C++20.

## Domain

A client brings in a broken device. The receptionist fills in an intake
act, recording the complaint and visible damage. A technician runs
diagnostics and prepares an estimate. The client approves the estimate,
after which it becomes immutable. The technician performs the work and
consumes the reserved spare parts from the warehouse; missing parts are
re-ordered from a supplier. When the work is finished an invoice is
issued, payment is accepted, the device is handed back and a warranty is
issued for the repair. Under warranty the client can come back again.

## Architecture

All classes follow one of five patterns. This is deliberate: any class is
explained by the pattern of its kind rather than as a special case.

| Pattern | Characteristics |
| --- | --- |
| Value object | immutable, no identity, invariants checked in the constructor |
| Entity | has identity, mutable state, invariants checked in methods |
| Catalog item | immutable description that entities refer to |
| Service | no data of its own, logic spanning several entities |
| Registry | a collection with lookup by key |

The domain is built into a static library `lab2_core`. The console
application and the tests link against it independently. Domain classes
know nothing about input/output: the menu works only through their
public interface and receives its streams from outside.

## Design decisions

**The client is stored via a smart pointer.** `RepairOrder` and the
registries keep the client as `std::shared_ptr<Client>` instead of by
value: storing by value would slice `CorporateClient` down to the base
class and lose the contract discount.

**Prices are frozen at the moment of the operation.** `WorkItem` and
`PartUsage` store the price as a copy: the catalog may get more
expensive, but an invoice that has already been issued must not change.

**Order transitions are checked against a table.** The state of
`RepairOrder` changes only through its methods, and the allowed
transitions are defined in `isTransitionAllowed`. Therefore a device
whose repair never started cannot be handed back.

**The calendar comes from `<chrono>`.** `Date` relies on the C++20
calendar, which removes manual leap-year handling.

## Classes

### Value objects — 9

Immutable, no identity. All invariants are checked in the constructor, comparison is by value.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `Address` | `city_`, `street_`, `house_`, `apartment_` | `isPrivateHouse`, `isSameCity`, `oneLine`, `operator==` | — |
| `Date` | `year_`, `month_`, `day_` | `isValid`, `daysUntil`, `plusDays`, `plusMonths`, `isBefore`, `isAfter`, `toString`, `operator==`, `operator<` | — |
| `Duration` | `minutes_` | `fromHours`, `totalMinutes`, `hours`, `minutesPart`, `isZero`, `operator+`, `isLongerThan`, `operator==`, `toString` | — |
| `EmailAddress` | `value_` | `isValid`, `localPart`, `domain`, `belongsToDomain`, `operator==` | — |
| `Money` | `kopecks_` | `fromRubles`, `rubles`, `kopecksPart`, `isZero`, `operator+`, `operator-`, `operator*`, `operator==`, `operator<`, `toString` | — |
| `Percentage` | `value_` | `isZero`, `applyTo`, `remainderOf`, `combinedWith`, `operator==`, `operator<` | `Money` |
| `PersonName` | `surname_`, `givenName_`, `patronymic_` | `hasPatronymic`, `fullName`, `shortName`, `operator==` | — |
| `PhoneNumber` | `digits_` | `isValid`, `operatorCode`, `formatted`, `operator==` | — |
| `SerialNumber` | `value_` | `isValid`, `batchPrefix`, `operator==`, `operator<` | — |

### Catalog items — 7

Descriptions that entities refer to: manufacturers, models, spare parts, service types and prices.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `DeviceCategory` | `name_`, `baseWarrantyMonths_` | `matchesName`, `warrantyMonthsFor`, `operator==` | — |
| `DeviceModel` | `manufacturer_`, `category_`, `modelName_`, `releaseYear_` | `fullName`, `ageInYears`, `isObsolete`, `operator==` | `Date`, `DeviceCategory`, `Manufacturer` |
| `Manufacturer` | `name_`, `country_` | `isDomesticFor`, `operator==` | — |
| `Part` | `article_`, `name_`, `price_`, `compatibleModels_` | `compatibleModelCount`, `declareCompatibleWith`, `isCompatibleWith`, `totalPriceFor`, `operator==` | `DeviceModel`, `Money` |
| `PriceList` | `entries_` | `size`, `isEmpty`, `contains`, `priceFor`, `totalFor`, `entryAt` | `Money`, `PriceListEntry`, `ServiceType` |
| `PriceListEntry` | `serviceType_`, `price_` | `totalFor`, `describes` | `Money`, `ServiceType` |
| `ServiceType` | `name_`, `normativeDuration_`, `requiresDiagnostics_` | `estimatedDurationFor`, `matchesName`, `operator==` | `Duration` |

### People — 5

Employees and clients. Two inheritance hierarchies: `Employee` and `Client`.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `Client` | `name_`, `phone_`, `email_`, `address_`, `completedOrders_` | `clientKind`, `loyaltyDiscount`, `registerCompletedOrder`, `isRegular`, `contactLine` | `Address`, `EmailAddress`, `Percentage`, `PersonName`, `PhoneNumber` |
| `CorporateClient`<br>*derives from* `Client` | `companyName_`, `contractNumber_`, `contractDiscount_` | `clientKind`, `loyaltyDiscount` | `Address`, `EmailAddress`, `Percentage`, `PersonName`, `PhoneNumber` |
| `Employee` | `name_`, `phone_`, `hireDate_`, `baseSalary_` | `position`, `monthlySalary`, `yearsOfService`, `isEligibleForSeniorityBonus` | `Date`, `Money`, `PersonName`, `PhoneNumber` |
| `Manager`<br>*derives from* `Employee` | `approvalLimit_`, `subordinateCount_` | `position`, `monthlySalary`, `canApprove`, `addSubordinate`, `removeSubordinate` | `Date`, `Money`, `PersonName`, `PhoneNumber` |
| `Technician`<br>*derives from* `Employee` | `specializations_`, `grade_`, `activeOrders_` | `position`, `monthlySalary`, `specializationCount`, `addSpecialization`, `canServe`, `isOverloaded`, `takeOrder`, `releaseOrder` | `Date`, `DeviceCategory`, `Money`, `PersonName`, `PhoneNumber` |

### Repair process — 11

The core of the model. The central entity is `RepairOrder` with a state machine.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `Defect` | `description_`, `unit_`, `severity_` | `isBlocking`, `isMoreSevereThan`, `operator==` | — |
| `Device` | `model_`, `serialNumber_`, `purchaseDate_`, `accessories_` | `accessoryCount`, `addAccessory`, `hasAccessory`, `isUnderFactoryWarranty`, `ageInDays`, `operator==` | `Date`, `DeviceModel`, `SerialNumber` |
| `Diagnostics` | `performedBy_`, `performedOn_`, `conclusion_`, `foundDefects_` | `defectCount`, `recordDefect`, `requiresRepair`, `mostSevereDefect`, `defectAt`, `complexityFactor`, `describe` | `Date`, `Defect` |
| `Estimate` | `works_`, `parts_`, `approvedOn_` | `workCount`, `partCount`, `isApproved`, `isEmpty`, `addWork`, `addPart`, `worksCost`, `partsCost`, `totalCost`, `approve`, `approvalDate`, `workAt`, `partAt` | `Date`, `Money`, `PartUsage`, `WorkItem` |
| `IntakeAct` | `device_`, `declaredProblem_`, `acceptedOn_`, `acceptedBy_`, `externalDefects_` | `externalDefectCount`, `recordExternalDefect`, `hasExternalDefect`, `hasBlockingDefect`, `describe` | `Date`, `Defect`, `Device` |
| `OrderHistoryEntry` | `fromStatus_`, `toStatus_`, `changedOn_`, `changedBy_` | `describe`, `leadsTo` | `Date` |
| `PartUsage` | `part_`, `quantity_`, `unitPrice_` | `totalCost`, `priceRoseSinceUsage`, `describe` | `Money`, `Part` |
| `RepairOrder` | `number_`, `client_`, `intakeAct_`, `status_`, `openedOn_`, `estimate_`, `history_`, `diagnostics_`, `warranty_`, `technician_` | `historySize`, `historyAt`, `isClosed`, `hasTechnician`, `assignTechnician`, `recordDiagnostics`, `addWorkToEstimate`, `addPartToEstimate`, `approveEstimate`, `startRepair`, `completeRepair`, `issueToClient`, `cancel`, `totalCost`, `changeStatus`, `isTransitionAllowed`, `actorName` | `Client`, `Date`, `Diagnostics`, `Estimate`, `IntakeAct`, `Money`, `OrderHistoryEntry`, `PartUsage`, `Technician`, `Warranty`, `WorkItem` |
| `Warranty` | `startDate_`, `months_`, `coveredUnits_` | `coveredUnitCount`, `expiryDate`, `addCoveredUnit`, `covers`, `isActiveOn`, `daysLeftOn` | `Date` |
| `WarrantyClaim` | `originalOrderNumber_`, `failedUnit_`, `submittedOn_`, `resolved_`, `accepted_`, `rejectionReason_` | `isResolved`, `isAccepted`, `matches`, `accept`, `reject` | `Date`, `Warranty` |
| `WorkItem` | `serviceType_`, `performedBy_`, `spentTime_`, `price_` | `isWithinNorm`, `overtime`, `describe` | `Duration`, `Money`, `ServiceType` |

### Warehouse and supplies — 6

Stock levels, reservations for orders and purchasing from suppliers.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `PartReservation` | `part_`, `quantity_`, `orderNumber_`, `createdOn_`, `active_` | `isActive`, `release`, `belongsToOrder`, `isStale`, `describe` | `Date`, `Part` |
| `StockItem` | `part_`, `quantity_`, `reserved_`, `location_` | `reservedQuantity`, `availableQuantity`, `receive`, `reserve`, `releaseReservation`, `writeOff`, `isBelowMinimum` | `Part` |
| `Supplier` | `name_`, `phone_`, `deliveryDays_`, `rating_` | `expectedDeliveryDate`, `isReliable`, `raiseRating`, `lowerRating`, `isPreferableTo` | `Date`, `PhoneNumber` |
| `SupplyOrder` | `number_`, `supplier_`, `createdOn_`, `items_`, `receivedOn_` | `itemCount`, `isReceived`, `expectedDate`, `addItem`, `itemAt`, `totalCost`, `expectedProfit`, `receive`, `wasDeliveredOnTime` | `Date`, `Money`, `Supplier`, `SupplyOrderItem`, `Warehouse` |
| `SupplyOrderItem` | `part_`, `quantity_`, `purchasePrice_` | `totalCost`, `expectedRevenue`, `expectedProfit`, `describe` | `Money`, `Part` |
| `Warehouse` | `items_` | `itemCount`, `receive`, `contains`, `availableQuantity`, `reserve`, `releaseReservation`, `writeOff`, `itemAt`, `partsToReorder`, `findItem`, `findItem` | `Part`, `StockItem` |

### Finance — 4

Invoices, payments and discounts.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `Discount` | `percentage_`, `reason_`, `minimumAmount_` | `appliesTo`, `amountFor`, `applyTo`, `isBetterThan` | `Money`, `Percentage` |
| `Invoice` | `number_`, `orderNumber_`, `issuedOn_`, `lines_`, `payments_`, `discount_` | `lineCount`, `paymentCount`, `addLine`, `applyDiscount`, `subtotal`, `discountAmount`, `total`, `paidAmount`, `balanceDue`, `isPaid`, `registerPayment`, `lineAt`, `paymentAt` | `Date`, `Discount`, `InvoiceLine`, `Money`, `Payment` |
| `InvoiceLine` | `description_`, `amount_` | `exceeds`, `describe` | `Money` |
| `Payment` | `amount_`, `method_`, `receivedOn_`, `reference_` | `isCashless`, `isOnTime`, `describe` | `Date`, `Money` |

### Scheduling — 3

Time intervals, technician schedules and client appointments.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `Appointment` | `client_`, `date_`, `slot_`, `purpose_`, `cancelled_` | `isCancelled`, `reschedule`, `cancel`, `conflictsWith`, `describe` | `Client`, `Date`, `TimeSlot` |
| `TimeSlot` | `startMinutes_`, `duration_` | `endMinutes`, `overlaps`, `startsBefore`, `contains`, `toString`, `operator==` | `Duration` |
| `WorkSchedule` | `technicianName_`, `date_`, `slots_` | `slotCount`, `occupy`, `isFree`, `slotAt`, `busyTime`, `remainingTime`, `hasCapacityFor` | `Date`, `Duration`, `TimeSlot` |

### Services — 5

Logic that ties several entities together. They have no mutable state of their own.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `InventoryService` | — | `canSupply`, `consume`, `cancelReservation` | `Date`, `Part`, `PartReservation`, `PartUsage`, `Supplier`, `SupplyOrder`, `Warehouse` |
| `OrderService` | — | `assignBestTechnician`, `amountDue`, `readyForIssue` | `Client`, `Date`, `EmployeeRegistry`, `IntakeAct`, `Invoice`, `Money`, `OrderBook`, `PricingService`, `RepairOrder` |
| `PricingService` | `priceList_` | `isPriceKnown`, `workCost`, `priceForClient`, `orderTotal` | `Client`, `Discount`, `Money`, `PriceList`, `RepairOrder`, `ServiceType` |
| `SchedulingService` | — | `canPlace`, `place`, `plannedTime` | `Date`, `DeviceCategory`, `Duration`, `EmployeeRegistry`, `Estimate`, `Technician`, `TimeSlot`, `WorkSchedule` |
| `WarrantyService` | — | `warrantyMonthsFor`, `isCoveredCase`, `process`, `coverageEnd` | `Date`, `RepairOrder`, `WarrantyClaim` |

### Registries — 4

Collections of entities with lookup by key.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `ClientRegistry` | `clients_` | `size`, `registerClient`, `contains`, `findByPhone`, `clientAt`, `regularClients` | `Client`, `PhoneNumber` |
| `EmployeeRegistry` | `technicians_`, `managers_` | `technicianCount`, `managerCount`, `hire`, `hire`, `technicianAt`, `managerAt`, `findAvailableFor`, `findApproverFor`, `monthlyPayroll` | `DeviceCategory`, `Manager`, `Money`, `Technician` |
| `OrderBook` | `orders_`, `nextNumber_` | `size`, `issueNumber`, `add`, `findByNumber`, `contains`, `orderAt`, `ordersWithStatus`, `openOrderCount` | `RepairOrder` |
| `PartCatalog` | `parts_` | `size`, `add`, `contains`, `findByArticle`, `partAt`, `partsFor`, `declareCompatibility` | `DeviceModel`, `Part` |

### Exceptions — 14

A hierarchy with a single root and three branches.

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `BusinessRuleException`<br>*derives from* `RepairException` | — | — | — |
| `ClientNotFoundException`<br>*derives from* `NotFoundException` | `phone_` | — | — |
| `EstimateNotApprovedException`<br>*derives from* `BusinessRuleException` | `orderNumber_` | — | — |
| `InvalidMoneyAmountException`<br>*derives from* `ValidationException` | `kopecks_` | — | — |
| `InvalidPhoneNumberException`<br>*derives from* `ValidationException` | `rawNumber_` | — | — |
| `InvalidSerialNumberException`<br>*derives from* `ValidationException` | `rawSerial_` | — | — |
| `InvalidStatusTransitionException`<br>*derives from* `BusinessRuleException` | `fromStatus_`, `toStatus_` | — | — |
| `NotFoundException`<br>*derives from* `RepairException` | — | — | — |
| `OrderNotFoundException`<br>*derives from* `NotFoundException` | `orderNumber_` | — | — |
| `OutOfStockException`<br>*derives from* `BusinessRuleException` | `article_`, `requested_`, `available_` | — | — |
| `PartNotFoundException`<br>*derives from* `NotFoundException` | `article_` | — | — |
| `RepairException`<br>*derives from* `std::runtime_error` | — | — | — |
| `ValidationException`<br>*derives from* `RepairException` | — | — | — |
| `WarrantyExpiredException`<br>*derives from* `BusinessRuleException` | `orderNumber_`, `expiryDate_` | — | — |

## Exception hierarchy

```
RepairException                          base class, derives from std::runtime_error
├── ValidationException                  invalid input data
│   ├── InvalidMoneyAmountException      negative amount of money
│   ├── InvalidPhoneNumberException      phone number does not match the format
│   └── InvalidSerialNumberException     serial number does not match the format
├── NotFoundException                    object not found
│   ├── ClientNotFoundException          no client registered with this phone number
│   ├── OrderNotFoundException           no order with this number in the order book
│   └── PartNotFoundException            no spare part with this article in the catalog
└── BusinessRuleException                a domain rule was violated
    ├── InvalidStatusTransitionException order status transition is not allowed
    ├── OutOfStockException              not enough spare parts in stock
    ├── WarrantyExpiredException         the warranty period of the order has expired
    └── EstimateNotApprovedException     the estimate has not been approved by the client
```

The three branches make it possible to catch errors in groups: invalid
input, a missing object and a violated domain rule are handled separately.

## Summary metrics

The numbers below are produced by the `tools/metrics.py` script, which
parses the header files. Getters and setters are excluded from the method
count: a method is considered a getter if its name equals the field name
without the trailing underscore.

```
Classes (without exceptions)   54 / 50    OK
Exception classes              14 / 12    OK
Fields                        168 / 150   OK
Meaningful methods            302 / 100   OK
Associations                  140 / 30    OK
```

Check it yourself (from the repository root):

```sh
python3 lab2/tools/metrics.py
```

The script exits with a non-zero code if any minimum is not met.

## Requirements

| Tool | Version | Purpose |
| --- | --- | --- |
| CMake | 3.20+ | build |
| g++ or clang++ | with C++20 support | compilation |
| gcovr | any | code coverage, optional |
| cppcheck | any | static analysis, optional |
| doxygen | any | documentation generation, optional |
| python3 | 3.8+ | metrics script, optional |

GoogleTest does not need to be installed — it is downloaded
automatically through `FetchContent` during configuration.

All commands below are run **from the repository root**.

## Build and run

```sh
cmake -S lab2 -B build2
cmake --build build2
./build2/lab2_app
```

Build and run in one line:

```sh
cmake -S lab2 -B build2 && cmake --build build2 && ./build2/lab2_app
```

Build and immediately run the tests in one line:

```sh
cmake -S lab2 -B build2 && cmake --build build2 && ctest --test-dir build2 --output-on-failure
```

Full rebuild from scratch. Build directories are fully generated, so
they can be deleted at any time:

```sh
rm -rf build2 build-cov2 build-asan2
cmake -S lab2 -B build2
cmake --build build2
```

## Demo for the defense

The application has a menu:

```
1. Register a client            (surname, name, phone, e-mail, city, street, house)
2. List clients
3. Accept a device for repair   (serial number, complaint)
4. List orders
5. Warehouse stock
6. Demo repair scenario
0. Exit
```

Item 6 needs at least one registered client (item 1). The whole path
of an order is shown: opening, technician assignment, diagnostics,
estimate, parts reservation, repair, invoice, payment and handing the
device back.

Run the whole scenario without typing anything — the input is piped in
(register a client, accept a device, list clients, orders and stock,
run the demo, exit):

```sh
printf '1\nIvanov\nIvan\n375291234567\nivan@example.com\nMinsk\nLenina\n5\n3\nWM-2024-0001\nDoes not spin\n2\n4\n5\n6\n0\n' | ./build2/lab2_app
```

Quick demo only (register a client, then run the scenario):

```sh
printf '1\nIvanov\nIvan\n375291234567\nivan@example.com\nMinsk\nLenina\n5\n6\n0\n' | ./build2/lab2_app
```

Note: the phone number must contain 12 digits (country code included);
the serial number is 4–24 characters long and may contain only letters,
digits and hyphens (for example `WM-2024-0001`).

## Prebuilt binary

Building is optional: the
[Releases](https://github.com/parislove2007/ppois-labs/releases) section
contains an executable for Linux x86-64. It is linked statically, so no
compiler, CMake or libraries are needed to run it. The release is
created by CI when a tag of the form `v*` is pushed; `TAG` below is the
name of that tag.

```sh
TAG=v2.0-lab2
curl -L -o lab2_app \
  https://github.com/parislove2007/ppois-labs/releases/download/$TAG/lab2_app
chmod +x lab2_app
./lab2_app
```

The `chmod +x` command is mandatory: GitHub serves the file without the
executable permission.

Create the tag that triggers the release:

```sh
git tag v2.0-lab2
git push origin v2.0-lab2
```

## Tests

```sh
ctest --test-dir build2 --output-on-failure
```

Run only a part of the tests by name:

```sh
ctest --test-dir build2 -R Money --output-on-failure
```

Run the test binary directly (shows every test case by name):

```sh
./build2/lab2_tests
./build2/lab2_tests --gtest_filter='RepairOrder*'
```

List all tests without running them:

```sh
ctest --test-dir build2 -N
```

## Code coverage

```sh
cmake -S lab2 -B build-cov2 -DENABLE_COVERAGE=ON
cmake --build build-cov2
ctest --test-dir build-cov2
gcovr --root . --filter 'lab2/src/' --print-summary build-cov2
```

The same in one line — build, run the tests and show the coverage:

```sh
cmake -S lab2 -B build-cov2 -DENABLE_COVERAGE=ON && cmake --build build-cov2 && ctest --test-dir build-cov2 && gcovr --root . --filter 'lab2/src/' --print-summary build-cov2
```

The same check with the threshold that CI uses — it fails if line
coverage is below 90%:

```sh
gcovr --root . --filter 'lab2/src/' --fail-under-line 90 --print-summary build-cov2
```

Detailed HTML report:

```sh
gcovr --root . --filter 'lab2/src/' --html-details coverage.html build-cov2
```

## Documentation

```sh
cd lab2 && doxygen Doxyfile
```

The result is `lab2/docs/html/index.html`.

## Static analysis

```sh
cppcheck --enable=warning,style --error-exitcode=1 \
         --suppress=missingIncludeSystem lab2/src lab2/include
```

## Memory leaks and undefined behavior check

```sh
cmake -S lab2 -B build-asan2 -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g"
cmake --build build-asan2
ctest --test-dir build-asan2 --output-on-failure
```

## Run all CI checks locally

Everything the CI pipeline does for lab 2, in one command:

```sh
cmake -S lab2 -B build-cov2 -DENABLE_COVERAGE=ON && cmake --build build-cov2 && ctest --test-dir build-cov2 --output-on-failure && gcovr --root . --filter 'lab2/src/' --fail-under-line 90 --print-summary build-cov2 && cppcheck --enable=warning,style --error-exitcode=1 --suppress=missingIncludeSystem lab2/src lab2/include && python3 lab2/tools/metrics.py
```
