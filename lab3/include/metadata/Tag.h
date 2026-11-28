#pragma once

#include <iosfwd>
#include <string>

namespace fs
{

/**
 * @brief Метка для группировки записей, например «работа» или «важное».
 */
class Tag
{
public:
    /**
     * @brief Создаёт метку.
     * @param label Текст метки.
     * @throws InvalidFileNameException Если текст пустой.
     */
    explicit Tag(std::string label);
    Tag(std::string label, std::string category, std::string color);

    /** @return Текст метки. */
    const std::string& label() const;

    /** @return Категория метки, по умолчанию «general». */
    const std::string& category() const;

    /** @return Цвет метки для интерфейса. */
    const std::string& color() const;

    bool operator==(const Tag& other) const;

private:
    std::string label_;
    std::string category_;
    std::string color_;
};

std::ostream& operator<<(std::ostream& out, const Tag& tag);

}
