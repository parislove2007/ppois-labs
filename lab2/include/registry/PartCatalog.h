#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "catalog/DeviceModel.h"
#include "catalog/Part.h"

namespace repair
{

/**
 * @brief Каталог запчастей сервисного центра.
 *
 * Артикул уникален: две записи с одним артикулом означали бы две
 * разные цены на одну деталь. Подбор по модели опирается на список
 * совместимости, который ведут мастера.
 */
class PartCatalog
{
public:
    /** @brief Создаёт пустой каталог. */
    PartCatalog();

    /** @return Количество запчастей в каталоге. */
    std::size_t size() const;

    /**
     * @brief Добавляет запчасть в каталог.
     * @param part Добавляемая запчасть.
     * @throws BusinessRuleException Если артикул уже занят.
     */
    void add(const Part& part);

    /**
     * @brief Проверяет наличие артикула в каталоге.
     * @param article Проверяемый артикул.
     * @return true, если запчасть есть.
     */
    bool contains(const std::string& article) const;

    /**
     * @brief Находит запчасть по артикулу.
     * @param article Артикул.
     * @return Найденная запчасть.
     * @throws PartNotFoundException Если запчасти нет в каталоге.
     */
    const Part& findByArticle(const std::string& article) const;

    /**
     * @brief Возвращает запчасть по порядковому номеру.
     * @param index Номер, начиная с нуля.
     * @return Запчасть.
     * @throws NotFoundException Если номер выходит за границы.
     */
    const Part& partAt(std::size_t index) const;

    /**
     * @brief Подбирает запчасти, совместимые с моделью.
     * @param model Модель техники.
     * @return Список совместимых запчастей.
     */
    std::vector<Part> partsFor(const DeviceModel& model) const;

    /**
     * @brief Объявляет совместимость запчасти с моделью.
     * @param article Артикул запчасти.
     * @param model Совместимая модель.
     * @throws PartNotFoundException Если запчасти нет в каталоге.
     */
    void declareCompatibility(const std::string& article, const DeviceModel& model);

private:
    std::vector<Part> parts_;
};

}
