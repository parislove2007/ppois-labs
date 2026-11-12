#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Производитель бытовой техники.
 *
 * Справочная запись: создаётся один раз и далее только читается.
 * На неё ссылаются модели техники.
 */
class Manufacturer
{
public:
    /**
     * @brief Создаёт запись о производителе.
     * @param name Название марки.
     * @param country Страна, где зарегистрирован производитель.
     * @throws ValidationException Если название или страна пусты.
     */
    Manufacturer(const std::string& name, const std::string& country);

    /** @return Название марки. */
    const std::string& name() const;

    /** @return Страна производителя. */
    const std::string& country() const;

    /**
     * @brief Проверяет, является ли производитель отечественным.
     * @param homeCountry Страна сервисного центра.
     * @return true, если производитель зарегистрирован в этой же стране.
     */
    bool isDomesticFor(const std::string& homeCountry) const;

    /** @brief Сравнивает производителей по названию и стране. */
    bool operator==(const Manufacturer& other) const;

private:
    std::string name_;
    std::string country_;
};

/** @brief Записывает производителя в поток в виде "Название (Страна)". */
std::ostream& operator<<(std::ostream& out, const Manufacturer& manufacturer);

}
