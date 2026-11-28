#pragma once

#include <map>
#include <string>

#include "value/FileSize.h"

namespace fs
{

/**
 * @brief Учёт дисковых квот по логинам.
 *
 * Каждый пользователь тратит общий пул байтов; превышение лимита
 * блокирует запись новых данных.
 */
class QuotaManager
{
public:
    /**
     * @brief Создаёт менеджер квот.
     * @param defaultQuota Лимит для пользователей без персональной квоты.
     */
    explicit QuotaManager(FileSize defaultQuota);

    /** @brief Назначает персональную квоту. */
    void setQuota(const std::string& login, FileSize limit);

    /** @return Персональная квота или лимит по умолчанию. */
    FileSize quotaOf(const std::string& login) const;

    /** @return Сколько байтов уже занято пользователем. */
    FileSize usedOf(const std::string& login) const;

    /**
     * @brief Начисляет записанные байты.
     * @throws QuotaExceededException Если лимит будет превышен.
     */
    void accountWritten(const std::string& login, FileSize delta);

    /** @brief Списывает освобождённые байты. */
    void accountFreed(const std::string& login, FileSize delta);

private:
    FileSize defaultQuota_;
    std::map<std::string, FileSize> quotas_;
    std::map<std::string, FileSize> used_;
};

}
