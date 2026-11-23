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

    /** @return Текст метки. */
    const std::string& label() const;

    bool operator==(const Tag& other) const;

private:
    std::string label_;
};

std::ostream& operator<<(std::ostream& out, const Tag& tag);

}
