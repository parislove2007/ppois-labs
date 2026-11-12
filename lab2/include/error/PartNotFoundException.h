#pragma once

#include <string>
#include "error/NotFoundException.h"

namespace repair
{

/**
 * @brief Запчасть с указанным артикулом отсутствует в каталоге.
 */
class PartNotFoundException : public NotFoundException
{
public:
    /**
     * @brief Создаёт исключение.
     * @param article Артикул, по которому шёл поиск.
     */
    explicit PartNotFoundException(const std::string& article);

    /** @return Артикул, по которому шёл поиск. */
    const std::string& article() const;

private:
    std::string article_;
};

}
