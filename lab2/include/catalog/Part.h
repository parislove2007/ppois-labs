#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "catalog/DeviceModel.h"
#include "value/Money.h"

namespace repair
{

/**
 * @brief Запчасть из каталога сервисного центра.
 *
 * Справочная запись с одним изменяемым свойством: список совместимых
 * моделей пополняется по мере того, как мастера подтверждают
 * совместимость на практике.
 */
class Part
{
public:
    /**
     * @brief Создаёт запчасть.
     * @param article Артикул, уникальный в каталоге.
     * @param name Название запчасти.
     * @param price Цена за одну штуку.
     * @throws ValidationException Если артикул или название пусты.
     */
    Part(const std::string& article, const std::string& name, const Money& price);

    /** @return Артикул запчасти. */
    const std::string& article() const;

    /** @return Название запчасти. */
    const std::string& name() const;

    /** @return Цена за одну штуку. */
    const Money& price() const;

    /** @return Количество моделей, с которыми совместима запчасть. */
    std::size_t compatibleModelCount() const;

    /**
     * @brief Добавляет модель в список совместимых.
     *
     * Повторное добавление той же модели игнорируется.
     * @param model Совместимая модель.
     */
    void declareCompatibleWith(const DeviceModel& model);

    /**
     * @brief Проверяет совместимость запчасти с моделью.
     * @param model Проверяемая модель.
     * @return true, если совместимость объявлена.
     */
    bool isCompatibleWith(const DeviceModel& model) const;

    /**
     * @brief Считает стоимость партии запчастей.
     * @param quantity Количество штук.
     * @return Суммарная стоимость.
     * @throws ValidationException Если количество не положительно.
     */
    Money totalPriceFor(int quantity) const;

    /** @brief Сравнивает запчасти по артикулу. */
    bool operator==(const Part& other) const;

private:
    std::string article_;
    std::string name_;
    Money price_;
    std::vector<DeviceModel> compatibleModels_;
};

/** @brief Записывает запчасть в поток в виде "артикул — название". */
std::ostream& operator<<(std::ostream& out, const Part& part);

}
