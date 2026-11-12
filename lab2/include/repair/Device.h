#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "catalog/DeviceModel.h"
#include "value/Date.h"
#include "value/SerialNumber.h"

namespace repair
{

/**
 * @brief Конкретный аппарат, принадлежащий клиенту.
 *
 * Отличается от DeviceModel тем, что описывает экземпляр, а не тип:
 * у него есть серийный номер, дата покупки и комплектация, с которой
 * его сдали в ремонт.
 */
class Device
{
public:
    /**
     * @brief Создаёт запись об устройстве.
     * @param model Модель устройства.
     * @param serialNumber Серийный номер.
     * @param purchaseDate Дата покупки.
     */
    Device(const DeviceModel& model, const SerialNumber& serialNumber,
           const Date& purchaseDate);

    /** @return Модель устройства. */
    const DeviceModel& model() const;

    /** @return Серийный номер. */
    const SerialNumber& serialNumber() const;

    /** @return Дата покупки. */
    const Date& purchaseDate() const;

    /** @return Количество принятых принадлежностей. */
    std::size_t accessoryCount() const;

    /**
     * @brief Добавляет принадлежность, сданную вместе с устройством.
     *
     * Повторное добавление той же принадлежности игнорируется.
     * @param accessory Название принадлежности.
     * @throws ValidationException Если название пусто.
     */
    void addAccessory(const std::string& accessory);

    /**
     * @brief Проверяет, сдавалась ли принадлежность вместе с устройством.
     * @param accessory Название принадлежности.
     * @return true, если принадлежность в комплекте.
     */
    bool hasAccessory(const std::string& accessory) const;

    /**
     * @brief Проверяет действие заводской гарантии на дату.
     * @param today Дата проверки.
     * @param factoryWarrantyMonths Срок заводской гарантии в месяцах.
     * @return true, если гарантия производителя ещё действует.
     */
    bool isUnderFactoryWarranty(const Date& today, int factoryWarrantyMonths) const;

    /**
     * @brief Считает возраст устройства на дату.
     * @param today Дата, на которую считается возраст.
     * @return Возраст в днях, не меньше нуля.
     */
    int ageInDays(const Date& today) const;

    /** @brief Сравнивает устройства по серийному номеру. */
    bool operator==(const Device& other) const;

private:
    DeviceModel model_;
    SerialNumber serialNumber_;
    Date purchaseDate_;
    std::vector<std::string> accessories_;
};

}
