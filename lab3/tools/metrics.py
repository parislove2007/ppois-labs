#!/usr/bin/env python3
"""Подсчёт метрик объектной модели по требованиям лабораторной работы №3.

Разбирает заголовочные файлы в lab3/include и считает классы, поля,
содержательные методы, ассоциации и классы исключений. Завершается
ненулевым кодом, если какой-либо минимум не выполнен.

Запуск из корня репозитория:

    python3 lab3/tools/metrics.py
"""

import pathlib
import re
import sys

MINIMUMS = {
    "classes": 50,
    "exceptions": 12,
    "fields": 150,
    "methods": 100,
    "associations": 30,
}

INCLUDE_ROOT = pathlib.Path(__file__).resolve().parent.parent / "include"

CLASS_PATTERN = re.compile(r"^class\s+(\w+)(?:\s*:\s*public\s+([\w:]+))?\s*$", re.MULTILINE)
FIELD_NAME_PATTERN = re.compile(r"(\w+_)\s*;\s*$")
METHOD_NAME_PATTERN = re.compile(r"(operator\s*[^\s(]+|~?\w+)\s*$")


def strip_comments(text):
    """Убирает блочные и строчные комментарии, чтобы не считать примеры из них."""
    without_block = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    return re.sub(r"//[^\n]*", "", without_block)


def class_bodies(text):
    """Возвращает пары «имя класса, базовый класс, тело» для каждого класса файла."""
    bodies = []
    for match in CLASS_PATTERN.finditer(text):
        start = text.find("{", match.end())
        if start == -1:
            continue
        depth = 0
        for position in range(start, len(text)):
            if text[position] == "{":
                depth += 1
            elif text[position] == "}":
                depth -= 1
                if depth == 0:
                    bodies.append((match.group(1), match.group(2) or "", text[start:position]))
                    break
    return bodies


def is_member_line(line):
    """Объявление члена класса: отступ ровно четыре пробела, строка завершена."""
    return line.startswith("    ") and not line.startswith("     ") and line.rstrip().endswith(";")


def collect_fields(body):
    """Имена полей класса: объявления без скобок, оканчивающиеся на подчёркивание."""
    names = []
    for line in body.splitlines():
        if not is_member_line(line) or "(" in line:
            continue
        match = FIELD_NAME_PATTERN.search(line)
        if match:
            names.append(match.group(1))
    return names


def collect_methods(body, class_name, field_names):
    """Содержательные методы: без конструкторов, деструкторов, геттеров и сеттеров.

    Метод считается геттером, если его имя совпадает с именем поля без
    завершающего подчёркивания, и сеттером, если начинается с set.
    """
    accessor_names = {name.rstrip("_") for name in field_names}
    methods = []
    for line in body.splitlines():
        if not is_member_line(line) or "(" not in line:
            continue
        match = METHOD_NAME_PATTERN.search(line[:line.index("(")])
        if not match:
            continue
        name = match.group(1).replace(" ", "")
        if name in (class_name, "~" + class_name):
            continue
        if name in accessor_names or name.startswith("set"):
            continue
        methods.append(name)
    return methods


def collect_associations(body, class_name, known_classes):
    """Классы проекта, встречающиеся как типы полей или параметров методов."""
    used = set()
    for other in known_classes:
        if other == class_name:
            continue
        if re.search(r"\b" + re.escape(other) + r"\b", body):
            used.add(other)
    return used


def main():
    headers = sorted(INCLUDE_ROOT.rglob("*.h"))
    if not headers:
        print("Заголовочные файлы не найдены:", INCLUDE_ROOT)
        return 1

    parsed = []
    for header in headers:
        text = strip_comments(header.read_text(encoding="utf-8"))
        for name, base, body in class_bodies(text):
            parsed.append({
                "name": name,
                "base": base,
                "body": body,
                "is_exception": header.parent.name == "error",
            })

    known_classes = {entry["name"] for entry in parsed}
    totals = {"classes": 0, "exceptions": 0, "fields": 0, "methods": 0, "associations": 0}

    for entry in parsed:
        fields = collect_fields(entry["body"])
        methods = collect_methods(entry["body"], entry["name"], fields)
        associations = collect_associations(entry["body"], entry["name"], known_classes)

        totals["fields"] += len(fields)
        totals["methods"] += len(methods)
        totals["associations"] += len(associations)
        if entry["is_exception"]:
            totals["exceptions"] += 1
        else:
            totals["classes"] += 1

    titles = {
        "classes": "Классы (без исключений)",
        "exceptions": "Классы исключений",
        "fields": "Поля",
        "methods": "Содержательные методы",
        "associations": "Ассоциации",
    }

    failed = False
    for key in ("classes", "exceptions", "fields", "methods", "associations"):
        actual = totals[key]
        required = MINIMUMS[key]
        verdict = "OK" if actual >= required else "НЕ ВЫПОЛНЕНО"
        if actual < required:
            failed = True
        print(f"{titles[key]:<26} {actual:>5} / {required:<5} {verdict}")

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
