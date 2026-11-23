#pragma once

#include <string>

#include "entry/File.h"

namespace fs
{

/**
 * @brief Текстовый файл с подсчётом строк и слов.
 */
class TextFile : public File
{
public:
    using File::File;

    std::string kind() const override;

    /** @return Количество строк в содержимом. */
    int lineCount() const;

    /** @return Количество слов в содержимом. */
    int wordCount() const;
};

}
