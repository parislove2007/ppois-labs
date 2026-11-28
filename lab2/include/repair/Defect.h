#pragma once

#include <iosfwd>
#include <string>

namespace repair
{

/**
 * @brief Неисправность, обнаруженная в устройстве.
 *
 * Критичность задаётся числом от одного до пяти: по ней решается,
 * можно ли выдать устройство клиенту без устранения дефекта.
 */
class Defect
{
public:
    /**
     * @brief Создаёт описание дефекта.
     * @param description Описание неисправности.
     * @param unit Узел устройства, в котором обнаружен дефект.
     * @param severity Критичность, от одного до пяти.
     * @throws ValidationException Если описание пусто или критичность вне диапазона.
     */
    Defect(const std::string& description, const std::string& unit, int severity);

    /** @return Описание неисправности. */
    const std::string& description() const;

    /** @return Узел устройства. */
    const std::string& unit() const;

    /** @return Критичность дефекта. */
    int severity() const;

    /** @return true, если дефект препятствует выдаче устройства. */
    bool isBlocking() const;

    /**
     * @brief Сравнивает дефекты по критичности.
     * @param other Дефект для сравнения.
     * @return true, если текущий дефект опаснее.
     */
    bool isMoreSevereThan(const Defect& other) const;

    /** @brief Сравнивает дефекты по описанию и узлу. */
    bool operator==(const Defect& other) const;

private:
    std::string description_;
    std::string unit_;
    int severity_;
};

/** @brief Записывает дефект в поток в виде "узел: описание (критичность N)". */
std::ostream& operator<<(std::ostream& out, const Defect& defect);

}
