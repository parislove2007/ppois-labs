#pragma once

#include <iosfwd>
#include <string>

namespace fs
{

/**
 * @brief Расширение имени файла без ведущей точки.
 *
 * Неизменяемый объект-значение; регистр не важен, «TXT» и «txt» —
 * одно и то же расширение.
 */
class FileExtension
{
public:
    /**
     * @brief Извлекает расширение из имени файла.
     * @param fileName Имя файла, например «report.txt».
     * @return Расширение в нижнем регистре; пустое, если точки нет.
     */
    static FileExtension fromFileName(const std::string& fileName);

    /** @brief Создаёт пустое расширение. */
    FileExtension();

    /** @return Расширение в нижнем регистре без точки. */
    const std::string& value() const;

    /** @return true, если расширение отсутствует. */
    bool isEmpty() const;

    /** @return true для текстовых типов (txt, md, csv, log). */
    bool isText() const;

    /** @return true для изображений (png, jpg, jpeg, gif, bmp). */
    bool isImage() const;

    /** @return true для архивов (zip, tar, gz, 7z). */
    bool isArchive() const;

    /** @return true, если расширение входит в известные категории. */
    bool isKnown() const;

    bool operator==(const FileExtension& other) const;

private:
    explicit FileExtension(std::string value);

    std::string value_;
};

std::ostream& operator<<(std::ostream& out, const FileExtension& extension);

}
