#pragma once

#include <vector>

#include "metadata/EntryVersion.h"

namespace fs
{

/**
 * @brief История версий одного файла.
 *
 * Откат не переписывает прошлое: содержимое старой версии сохраняется
 * как новая версия — история остаётся честной, как в системах контроля
 * версий.
 */
class VersionHistory
{
public:
    /** @brief Создаёт пустую историю без лимита версий. */
    VersionHistory();

    /**
     * @brief Создаёт историю с лимитом версий.
     * @param maxVersions Сколько версий хранить; ноль — без лимита.
     */
    explicit VersionHistory(std::size_t maxVersions);

    /**
     * @brief Сохраняет новую версию содержимого.
     * @param content Содержимое.
     * @param at Момент сохранения.
     * @return Созданная версия с очередным номером.
     */
    EntryVersion addVersion(std::string content, Timestamp at);

    /** @return Количество версий. */
    std::size_t count() const;

    /**
     * @brief Возвращает последнюю версию.
     * @throws VersionConflictException Если история пуста.
     */
    const EntryVersion& latest() const;

    /**
     * @brief Ищет версию по номеру.
     * @throws VersionConflictException Если версии с таким номером нет.
     */
    const EntryVersion& findByNumber(int number) const;

    /**
     * @brief Откатывается к версии: её содержимое сохраняется как новая версия.
     * @param number Номер версии, к которой откатываемся.
     * @param at Момент отката.
     * @return Созданная версия-откат.
     * @throws VersionConflictException Если версии с таким номером нет.
     */
    EntryVersion rollbackTo(int number, Timestamp at);

private:
    void trimToLimit();

    std::vector<EntryVersion> versions_;
    std::size_t maxVersions_;
};

}
