#pragma once

#include "value/Timestamp.h"

namespace fs
{

/**
 * @brief Доменные часы: выдают монотонно растущие метки времени.
 *
 * Замена системного времени: все операции получают момент от часов,
 * что делает поведение детерминированным и тестируемым.
 */
class Clock
{
public:
    explicit Clock(std::int64_t startSeconds, std::int64_t stepSeconds = 1);

    /** @brief Возвращает текущую метку и сдвигает часы на шаг. */
    Timestamp next();

    /** @return Текущая метка без сдвига. */
    Timestamp now() const;

private:
    std::int64_t current_;
    std::int64_t stepSeconds_;
};

}
