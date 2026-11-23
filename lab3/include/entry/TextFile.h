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
    TextFile(std::string name, std::string owner, Permission permission, Timestamp created);

    std::string kind() const override;

    /** @return Количество строк в содержимом. */
    int lineCount() const;

    /** @return Количество слов в содержимом. */
    int wordCount() const;

    /** @return Кодировка текста, по умолчанию «utf-8». */
    const std::string& encoding() const;

    /** @brief Меняет кодировку текста. */
    void setEncoding(std::string encoding);

    /** @return Стиль переводов строк: «lf» или «crlf». */
    const std::string& lineEnding() const;
    void setLineEnding(std::string lineEnding);

private:
    std::string encoding_;
    std::string lineEnding_;
};

}
