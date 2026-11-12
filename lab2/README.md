# Лабораторная работа 2 — сервисный центр по ремонту бытовой техники

Объектная модель предметной области «фирма по ремонту» на C++20.

## Предметная область

Клиент приносит неисправное устройство. Приёмщик оформляет акт приёмки,
фиксируя жалобу и внешние повреждения. Мастер проводит диагностику и
составляет смету. Клиент утверждает смету, после чего она становится
неизменной. Мастер выполняет работы и списывает со склада
зарезервированные запчасти; недостающие дозаказываются у поставщика.
По завершении выставляется счёт, принимается оплата, устройство
выдаётся и на ремонт выдаётся гарантия. По гарантии клиент может
обратиться повторно.

## Архитектура

Все классы сводятся к пяти шаблонам. Это сделано намеренно: любой класс
объясняется по шаблону своего вида, а не как отдельный случай.

| Шаблон | Признаки |
| --- | --- |
| Объект-значение | неизменяемый, без идентичности, инварианты в конструкторе |
| Сущность | имеет идентичность, изменяемое состояние, инварианты в методах |
| Справочник | неизменяемое описание, на него ссылаются сущности |
| Сервис | без собственных данных, логика между несколькими сущностями |
| Реестр | коллекция с поиском по ключу |

Предметная область собрана в статическую библиотеку `lab2_core`.
Консольное приложение и тесты подключаются к ней независимо, а классы
домена ничего не знают о вводе-выводе: меню работает только через их
публичный интерфейс и получает потоки извне.

## Решения, объясняющие структуру

**Клиент хранится умным указателем.** В `RepairOrder` и реестрах клиент
хранится как `std::shared_ptr<Client>`, а не по значению: хранение по
значению срезало бы `CorporateClient` до базового класса и потеряло бы
договорную скидку.

**Цены фиксируются в момент операции.** `WorkItem` и `PartUsage` хранят
цену копией: каталог может подорожать, но уже выставленный счёт от
этого меняться не должен.

**Переходы заказа проверяются по таблице.** Состояние `RepairOrder`
меняется только через его методы, а допустимость перехода задана в
`isTransitionAllowed`. Поэтому нельзя выдать устройство, ремонт
которого не начинался.

**Календарь взят из `<chrono>`.** Класс `Date` опирается на
календарь C++20, что избавляет от ручной обработки високосных годов.

## Классы

### Объекты-значения — 9

Неизменяемые, без идентичности. Все инварианты проверяются в конструкторе, сравнение идёт по значению.

| Класс | Поля | Методы | Связанные классы |
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

### Справочники — 7

Описания, на которые ссылаются сущности: производители, модели, запчасти, виды работ и цены.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `DeviceCategory` | `name_`, `baseWarrantyMonths_` | `matchesName`, `warrantyMonthsFor`, `operator==` | — |
| `DeviceModel` | `manufacturer_`, `category_`, `modelName_`, `releaseYear_` | `fullName`, `ageInYears`, `isObsolete`, `operator==` | `Date`, `DeviceCategory`, `Manufacturer` |
| `Manufacturer` | `name_`, `country_` | `isDomesticFor`, `operator==` | — |
| `Part` | `article_`, `name_`, `price_`, `compatibleModels_` | `compatibleModelCount`, `declareCompatibleWith`, `isCompatibleWith`, `totalPriceFor`, `operator==` | `DeviceModel`, `Money` |
| `PriceList` | `entries_` | `size`, `isEmpty`, `contains`, `priceFor`, `totalFor`, `entryAt` | `Money`, `PriceListEntry`, `ServiceType` |
| `PriceListEntry` | `serviceType_`, `price_` | `totalFor`, `describes` | `Money`, `ServiceType` |
| `ServiceType` | `name_`, `normativeDuration_`, `requiresDiagnostics_` | `estimatedDurationFor`, `matchesName`, `operator==` | `Duration` |

### Люди — 5

Сотрудники и клиенты. Две иерархии наследования: `Employee` и `Client`.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `Client` | `name_`, `phone_`, `email_`, `address_`, `completedOrders_` | `clientKind`, `loyaltyDiscount`, `registerCompletedOrder`, `isRegular`, `contactLine` | `Address`, `EmailAddress`, `Percentage`, `PersonName`, `PhoneNumber` |
| `CorporateClient`<br>*наследник* `Client` | `companyName_`, `contractNumber_`, `contractDiscount_` | `clientKind`, `loyaltyDiscount` | `Address`, `EmailAddress`, `Percentage`, `PersonName`, `PhoneNumber` |
| `Employee` | `name_`, `phone_`, `hireDate_`, `baseSalary_` | `position`, `monthlySalary`, `yearsOfService`, `isEligibleForSeniorityBonus` | `Date`, `Money`, `PersonName`, `PhoneNumber` |
| `Manager`<br>*наследник* `Employee` | `approvalLimit_`, `subordinateCount_` | `position`, `monthlySalary`, `canApprove`, `addSubordinate`, `removeSubordinate` | `Date`, `Money`, `PersonName`, `PhoneNumber` |
| `Technician`<br>*наследник* `Employee` | `specializations_`, `grade_`, `activeOrders_` | `position`, `monthlySalary`, `specializationCount`, `addSpecialization`, `canServe`, `isOverloaded`, `takeOrder`, `releaseOrder` | `Date`, `DeviceCategory`, `Money`, `PersonName`, `PhoneNumber` |

### Процесс ремонта — 11

Ядро модели. Центральная сущность — `RepairOrder` с машиной состояний.

| Класс | Поля | Методы | Связанные классы |
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

### Склад и поставки — 6

Остатки, резервы под заказы и закупка у поставщиков.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `PartReservation` | `part_`, `quantity_`, `orderNumber_`, `createdOn_`, `active_` | `isActive`, `release`, `belongsToOrder`, `isStale`, `describe` | `Date`, `Part` |
| `StockItem` | `part_`, `quantity_`, `reserved_`, `location_` | `reservedQuantity`, `availableQuantity`, `receive`, `reserve`, `releaseReservation`, `writeOff`, `isBelowMinimum` | `Part` |
| `Supplier` | `name_`, `phone_`, `deliveryDays_`, `rating_` | `expectedDeliveryDate`, `isReliable`, `raiseRating`, `lowerRating`, `isPreferableTo` | `Date`, `PhoneNumber` |
| `SupplyOrder` | `number_`, `supplier_`, `createdOn_`, `items_`, `receivedOn_` | `itemCount`, `isReceived`, `expectedDate`, `addItem`, `itemAt`, `totalCost`, `expectedProfit`, `receive`, `wasDeliveredOnTime` | `Date`, `Money`, `Supplier`, `SupplyOrderItem`, `Warehouse` |
| `SupplyOrderItem` | `part_`, `quantity_`, `purchasePrice_` | `totalCost`, `expectedRevenue`, `expectedProfit`, `describe` | `Money`, `Part` |
| `Warehouse` | `items_` | `itemCount`, `receive`, `contains`, `availableQuantity`, `reserve`, `releaseReservation`, `writeOff`, `itemAt`, `partsToReorder`, `findItem`, `findItem` | `Part`, `StockItem` |

### Финансы — 4

Счета, платежи и скидки.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `Discount` | `percentage_`, `reason_`, `minimumAmount_` | `appliesTo`, `amountFor`, `applyTo`, `isBetterThan` | `Money`, `Percentage` |
| `Invoice` | `number_`, `orderNumber_`, `issuedOn_`, `lines_`, `payments_`, `discount_` | `lineCount`, `paymentCount`, `addLine`, `applyDiscount`, `subtotal`, `discountAmount`, `total`, `paidAmount`, `balanceDue`, `isPaid`, `registerPayment`, `lineAt`, `paymentAt` | `Date`, `Discount`, `InvoiceLine`, `Money`, `Payment` |
| `InvoiceLine` | `description_`, `amount_` | `exceeds`, `describe` | `Money` |
| `Payment` | `amount_`, `method_`, `receivedOn_`, `reference_` | `isCashless`, `isOnTime`, `describe` | `Date`, `Money` |

### Планирование — 3

Интервалы времени, графики мастеров и запись клиентов.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `Appointment` | `client_`, `date_`, `slot_`, `purpose_`, `cancelled_` | `isCancelled`, `reschedule`, `cancel`, `conflictsWith`, `describe` | `Client`, `Date`, `TimeSlot` |
| `TimeSlot` | `startMinutes_`, `duration_` | `endMinutes`, `overlaps`, `startsBefore`, `contains`, `toString`, `operator==` | `Duration` |
| `WorkSchedule` | `technicianName_`, `date_`, `slots_` | `slotCount`, `occupy`, `isFree`, `slotAt`, `busyTime`, `remainingTime`, `hasCapacityFor` | `Date`, `Duration`, `TimeSlot` |

### Сервисы — 5

Логика, связывающая несколько сущностей. Собственного изменяемого состояния не имеют.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `InventoryService` | — | `canSupply`, `consume`, `cancelReservation` | `Date`, `Part`, `PartReservation`, `PartUsage`, `Supplier`, `SupplyOrder`, `Warehouse` |
| `OrderService` | — | `assignBestTechnician`, `amountDue`, `readyForIssue` | `Client`, `Date`, `EmployeeRegistry`, `IntakeAct`, `Invoice`, `Money`, `OrderBook`, `PricingService`, `RepairOrder` |
| `PricingService` | `priceList_` | `isPriceKnown`, `workCost`, `priceForClient`, `orderTotal` | `Client`, `Discount`, `Money`, `PriceList`, `RepairOrder`, `ServiceType` |
| `SchedulingService` | — | `canPlace`, `place`, `plannedTime` | `Date`, `DeviceCategory`, `Duration`, `EmployeeRegistry`, `Estimate`, `Technician`, `TimeSlot`, `WorkSchedule` |
| `WarrantyService` | — | `warrantyMonthsFor`, `isCoveredCase`, `process`, `coverageEnd` | `Date`, `RepairOrder`, `WarrantyClaim` |

### Реестры — 4

Коллекции сущностей с поиском по ключу.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `ClientRegistry` | `clients_` | `size`, `registerClient`, `contains`, `findByPhone`, `clientAt`, `regularClients` | `Client`, `PhoneNumber` |
| `EmployeeRegistry` | `technicians_`, `managers_` | `technicianCount`, `managerCount`, `hire`, `hire`, `technicianAt`, `managerAt`, `findAvailableFor`, `findApproverFor`, `monthlyPayroll` | `DeviceCategory`, `Manager`, `Money`, `Technician` |
| `OrderBook` | `orders_`, `nextNumber_` | `size`, `issueNumber`, `add`, `findByNumber`, `contains`, `orderAt`, `ordersWithStatus`, `openOrderCount` | `RepairOrder` |
| `PartCatalog` | `parts_` | `size`, `add`, `contains`, `findByArticle`, `partAt`, `partsFor`, `declareCompatibility` | `DeviceModel`, `Part` |

### Исключения — 14

Иерархия с одним корнем и тремя ветвями.

| Класс | Поля | Методы | Связанные классы |
| --- | --- | --- | --- |
| `BusinessRuleException`<br>*наследник* `RepairException` | — | — | — |
| `ClientNotFoundException`<br>*наследник* `NotFoundException` | `phone_` | — | — |
| `EstimateNotApprovedException`<br>*наследник* `BusinessRuleException` | `orderNumber_` | — | — |
| `InvalidMoneyAmountException`<br>*наследник* `ValidationException` | `kopecks_` | — | — |
| `InvalidPhoneNumberException`<br>*наследник* `ValidationException` | `rawNumber_` | — | — |
| `InvalidSerialNumberException`<br>*наследник* `ValidationException` | `rawSerial_` | — | — |
| `InvalidStatusTransitionException`<br>*наследник* `BusinessRuleException` | `fromStatus_`, `toStatus_` | — | — |
| `NotFoundException`<br>*наследник* `RepairException` | — | — | — |
| `OrderNotFoundException`<br>*наследник* `NotFoundException` | `orderNumber_` | — | — |
| `OutOfStockException`<br>*наследник* `BusinessRuleException` | `article_`, `requested_`, `available_` | — | — |
| `PartNotFoundException`<br>*наследник* `NotFoundException` | `article_` | — | — |
| `RepairException`<br>*наследник* `std::runtime_error` | — | — | — |
| `ValidationException`<br>*наследник* `RepairException` | — | — | — |
| `WarrantyExpiredException`<br>*наследник* `BusinessRuleException` | `orderNumber_`, `expiryDate_` | — | — |

## Иерархия исключений

```
RepairException                          базовое, наследник std::runtime_error
├── ValidationException                  некорректные входные данные
│   ├── InvalidMoneyAmountException      отрицательная денежная сумма
│   ├── InvalidPhoneNumberException      телефон не соответствует формату
│   └── InvalidSerialNumberException     серийный номер не соответствует формату
├── NotFoundException                    объект не найден
│   ├── ClientNotFoundException          клиент с таким телефоном не зарегистрирован
│   ├── OrderNotFoundException           заказа с таким номером нет в журнале
│   └── PartNotFoundException            запчасти с таким артикулом нет в каталоге
└── BusinessRuleException                нарушено правило предметной области
    ├── InvalidStatusTransitionException недопустимый переход заказа между состояниями
    ├── OutOfStockException              на складе недостаточно запчастей
    ├── WarrantyExpiredException         срок гарантии по заказу истёк
    └── EstimateNotApprovedException     смета не утверждена клиентом
```

Три ветви позволяют перехватывать ошибки группами: отдельно неверный
ввод, отдельно отсутствие объекта, отдельно нарушение правил домена.

## Итоговые метрики

Числа ниже получены скриптом `tools/metrics.py`, который разбирает
заголовочные файлы. Геттеры и сеттеры из числа методов исключены:
метод считается геттером, если его имя совпадает с именем поля без
завершающего подчёркивания.

```
Классы (без исключений)       54 / 50    OK
Классы исключений             14 / 12    OK
Поля                         168 / 150   OK
Содержательные методы        302 / 100   OK
Ассоциации                   140 / 30    OK
```

Проверить самостоятельно:

```sh
python3 lab2/tools/metrics.py
```

Скрипт завершается ненулевым кодом, если любой минимум не выполнен.

## Сборка и запуск

```sh
cmake -S lab2 -B build
cmake --build build
./build/lab2_app
```

GoogleTest подтягивается автоматически через `FetchContent`, ставить
его отдельно не нужно.

В приложении есть пункт «Демонстрационный сценарий ремонта», который
прогоняет весь путь заказа: открытие, назначение мастера, диагностика,
смета, резерв запчастей, ремонт, счёт, оплата и выдача.

## Тесты

```sh
ctest --test-dir build --output-on-failure
```

## Покрытие кода

```sh
cmake -S lab2 -B build-cov -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov
gcovr --root . --filter 'lab2/src/' --fail-under-line 90 --print-summary build-cov
```

## Статический анализ

```sh
cppcheck --enable=warning,style --error-exitcode=1 \
         --suppress=missingIncludeSystem lab2/src lab2/include
```
