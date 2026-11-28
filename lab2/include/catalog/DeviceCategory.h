#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Категория техники: стиральные машины, холодильники и прочее.
 *
 * Справочная запись. Категория задаёт базовый срок гарантии на ремонт
 * и используется для подбора мастера подходящей специализации.
 */
class DeviceCategory
{
public:
    /**
     * @brief Создаёт категорию.
     * @param name Название категории.
     * @param baseWarrantyMonths Базовый срок гарантии на ремонт, в месяцах.
     * @throws ValidationException Если название пусто или срок отрицателен.
     */
    DeviceCategory(const std::string& name, int baseWarrantyMonths);

    /** @return Название категории. */
    const std::string& name() const;

    /** @return Базовый срок гарантии в месяцах. */
    int baseWarrantyMonths() const;

    /**
     * @brief Сравнивает название категории с заданным без учёта регистра.
     * @param candidate Проверяемое название.
     * @return true, если названия совпадают.
     */
    bool matchesName(const std::string& candidate) const;

    /**
     * @brief Вычисляет срок гарантии с учётом сложности ремонта.
     * @param complexityFactor Множитель сложности, от единицы и выше.
     * @return Срок гарантии в месяцах.
     * @throws ValidationException Если множитель меньше единицы.
     */
    int warrantyMonthsFor(int complexityFactor) const;

    /** @brief Сравнивает категории по названию. */
    bool operator==(const DeviceCategory& other) const;

private:
    std::string name_;
    int baseWarrantyMonths_;
};

/** @brief Записывает категорию в поток. */
std::ostream& operator<<(std::ostream& out, const DeviceCategory& category);

}
