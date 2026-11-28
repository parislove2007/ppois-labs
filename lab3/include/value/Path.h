#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace fs
{

/**
 * @brief Путь к записи в файловой системе.
 *
 * Неизменяемый объект-значение. Разделитель сегментов — «/».
 * Путь обязан быть нормализованным: без пустых сегментов, точек
 * «.» и «..», без слэша в конце (кроме корня «/»).
 */
class Path
{
public:
    /**
     * @brief Разбирает путь из строки.
     * @param raw Исходная строка, например «/docs/report.txt».
     * @throws InvalidFileNameException Если путь пуст, содержит пустые
     *         сегменты («//», хвостовой «/») или сегменты «.» и «..».
     */
    static Path parse(const std::string& raw);

    /** @brief Создаёт корневой путь «/». */
    Path();

    /** @return Нормализованное строковое представление. */
    const std::string& toString() const;

    /** @return Имя последнего сегмента; для корня — «/». */
    std::string name() const;

    /** @return Путь родителя; родитель корня — сам корень. */
    Path parent() const;

    /** @return Расширение последнего сегмента без точки, пусто, если его нет. */
    std::string extension() const;

    /** @return true, если путь начинается с корня. */
    bool isAbsolute() const;

    /** @return Сегменты пути без разделителей; корень даёт пустой вектор. */
    std::vector<std::string> segments() const;

    /**
     * @brief Присоединяет относительный путь к текущему.
     * @param child Присоединяемый путь, обязан быть относительным.
     * @throws InvalidFileNameException Если дочерний путь абсолютный.
     */
    Path join(const Path& child) const;

    /**
     * @brief Сравнивает пути на равенство.
     * @param other Другой путь.
     * @return true, если нормализованные представления совпадают.
     */
    bool operator==(const Path& other) const;

private:
    explicit Path(std::string value);

    std::string value_;
};

/**
 * @brief Записывает путь в поток.
 * @param out Поток вывода.
 * @param path Записываемый путь.
 * @return Ссылка на поток.
 */
std::ostream& operator<<(std::ostream& out, const Path& path);

}
