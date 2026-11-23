#pragma once

#include <string>

#include "entry/File.h"

namespace fs
{

/**
 * @brief Бинарный файл с поддержкой дозаписи в конец.
 */
class BinaryFile : public File
{
public:
    using File::File;

    std::string kind() const override;

    /**
     * @brief Дописывает блок данных в конец файла.
     * @param chunk Дописываемые байты.
     * @param at Момент изменения.
     * @throws PermissionDeniedException Если нет права записи.
     */
    void append(const std::string& chunk, Timestamp at);
};

}
