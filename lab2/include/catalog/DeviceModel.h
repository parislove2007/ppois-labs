#pragma once

#include <iosfwd>
#include <string>

#include "catalog/DeviceCategory.h"
#include "catalog/Manufacturer.h"
#include "value/Date.h"

namespace repair
{

/**
 * @brief Модель техники: связка производителя, категории и названия.
 *
 * Справочная запись. Конкретный аппарат клиента описывается классом
 * Device и ссылается на модель.
 */
class DeviceModel
{
public:
    /**
     * @brief Создаёт модель.
     * @param manufacturer Производитель.
     * @param category Категория техники.
     * @param modelName Обозначение модели.
     * @param releaseYear Год начала выпуска.
     * @throws ValidationException Если обозначение пусто или год неправдоподобен.
     */
    DeviceModel(const Manufacturer& manufacturer, const DeviceCategory& category,
                const std::string& modelName, int releaseYear);

    /** @return Производитель. */
    const Manufacturer& manufacturer() const;

    /** @return Категория техники. */
    const DeviceCategory& category() const;

    /** @return Обозначение модели. */
    const std::string& modelName() const;

    /** @return Год начала выпуска. */
    int releaseYear() const;

    /** @return Полное название в виде "Марка Модель, категория". */
    std::string fullName() const;

    /**
     * @brief Считает возраст модели на указанную дату.
     * @param today Дата, на которую считается возраст.
     * @return Возраст модели в годах, не меньше нуля.
     */
    int ageInYears(const Date& today) const;

    /**
     * @brief Проверяет, снята ли модель с поддержки по возрасту.
     * @param today Дата проверки.
     * @param supportedYears Срок поддержки в годах.
     * @return true, если модель старше срока поддержки.
     */
    bool isObsolete(const Date& today, int supportedYears) const;

    /** @brief Сравнивает модели по производителю и обозначению. */
    bool operator==(const DeviceModel& other) const;

private:
    Manufacturer manufacturer_;
    DeviceCategory category_;
    std::string modelName_;
    int releaseYear_;
};

/** @brief Записывает модель в поток в полном виде. */
std::ostream& operator<<(std::ostream& out, const DeviceModel& model);

}
