#pragma once

#include <cstddef>
#include <iosfwd>
#include <vector>
#include "set_element.h"

/**
 * @brief Неориентированное канторовское множество.
 *
 * Элементы не повторяются и не упорядочены: множества с одними
 * и теми же элементами равны независимо от порядка добавления.
 * Элементом может быть как атом, так и другое множество.
 */
class Set
{
public:
    /** @brief Создаёт пустое множество. */
    Set();

    /** @return true, если множество не содержит элементов. */
    bool isEmpty() const;

    /** @return Мощность множества, то есть количество элементов. */
    std::size_t size() const;

    /**
     * @brief Возвращает элемент по его позиции в хранилище.
     *
     * Порядок элементов не является частью смысла множества
     * и используется только для обхода, например при выводе.
     * @param index Позиция элемента.
     * @pre index меньше size().
     * @return Ссылка на элемент.
     */
    const SetElement& at(std::size_t index) const;

    /**
     * @brief Проверяет принадлежность элемента множеству.
     * @param element Искомый элемент.
     * @return true, если элемент принадлежит множеству.
     */
    bool contains(const SetElement& element) const;

    /**
     * @brief Проверяет принадлежность элемента множеству.
     * @param element Искомый элемент.
     * @return true, если элемент принадлежит множеству.
     */
    bool operator[](const SetElement& element) const;

    /**
     * @brief Добавляет элемент, если его ещё нет.
     *
     * Повторное добавление существующего элемента не меняет множество.
     * @param element Добавляемый элемент.
     */
    void add(const SetElement& element);

    /**
     * @brief Удаляет элемент, если он присутствует.
     *
     * Удаление отсутствующего элемента не меняет множество.
     * @param element Удаляемый элемент.
     */
    void remove(const SetElement& element);

    /**
     * @brief Сравнивает множества на равенство без учёта порядка.
     * @param other Множество для сравнения.
     * @return true, если множества состоят из одних и тех же элементов.
     */
    bool operator==(const Set& other) const;

    /**
     * @brief Заменяет множество его объединением с другим.
     * @param other Второе множество.
     * @return Ссылка на изменённое множество.
     */
    Set& operator+=(const Set& other);

    /**
     * @brief Строит объединение двух множеств.
     * @param other Второе множество.
     * @return Новое множество; операнды не изменяются.
     */
    Set operator+(const Set& other) const;

    /**
     * @brief Заменяет множество его пересечением с другим.
     * @param other Второе множество.
     * @return Ссылка на изменённое множество.
     */
    Set& operator*=(const Set& other);

    /**
     * @brief Строит пересечение двух множеств.
     * @param other Второе множество.
     * @return Новое множество; операнды не изменяются.
     */
    Set operator*(const Set& other) const;

    /**
     * @brief Заменяет множество его разностью с другим.
     * @param other Вычитаемое множество.
     * @return Ссылка на изменённое множество.
     */
    Set& operator-=(const Set& other);

    /**
     * @brief Строит разность двух множеств.
     * @param other Вычитаемое множество.
     * @return Новое множество; операнды не изменяются.
     */
    Set operator-(const Set& other) const;

    /**
     * @brief Строит булеан — множество всех подмножеств.
     *
     * Результат содержит 2^n элементов, где n — мощность исходного
     * множества, поэтому размер источника ограничен.
     * @return Множество, элементами которого являются все подмножества.
     * @throws std::length_error Если мощность превышает maxPowerSetSource.
     */
    Set powerSet() const;

private:
    /**
     * @brief Добавляет элемент без проверки на дубликат.
     *
     * Используется там, где уникальность гарантирована построением,
     * например при переборе подмножеств в powerSet().
     * @param element Добавляемый элемент.
     */
    void appendUnchecked(const SetElement& element);

    std::vector<SetElement> elements_;

    /** @brief Наибольшая мощность множества, для которого строится булеан. */
    static const std::size_t maxPowerSetSource = 20;
};

/**
 * @brief Записывает множество в поток в формате "{a, b, {c}}".
 * @param out Поток вывода.
 * @param set Записываемое множество.
 * @return Ссылка на поток.
 */
std::ostream& operator<<(std::ostream& out, const Set& set);