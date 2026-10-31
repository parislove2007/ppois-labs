# ppois-labs

Лабораторные работы по курсу «Проектирование программного обеспечения
интеллектуальных систем». Язык реализации — C++20.

| Работа | Тема | Статус |
| --- | --- | --- |
| [lab1](lab1) | Основы ООП: разработка класса | готова |

## Лабораторная работа 1

Вариант 1.3 «Прямоугольник» и вариант 1.4 / 1.4.1 «Множество».

**Прямоугольник** — стороны параллельны осям, координаты целочисленные.
Перемещение, изменение размера, пре- и постинкремент и декремент,
наименьший охватывающий прямоугольник (`+`, `+=`), пересечение
(`-`, `-=`) с поддержкой пустого результата.

**Множество** — неориентированное канторовское: элементы не повторяются
и не упорядочены, элементом может быть другое множество. Добавление
и удаление, мощность, проверка принадлежности (`[]`), объединение
(`+`, `+=`), пересечение (`*`, `*=`), разность (`-`, `-=`), булеан,
разбор из строки вида `{a, b, {a, b}, {}, {a, {c}}}`.

### Структура

    lab1/
      include/   объявления классов
      src/       реализация
      app/       консольное приложение с меню
      tests/     модульные тесты
      Doxyfile   конфигурация генератора документации

Предметная область собирается в статическую библиотеку `lab1_core`,
к которой независимо подключаются приложение и тесты. Консольный
интерфейс отделён от реализации классов: меню работает только через
их публичный интерфейс.

### Требования

| Инструмент | Версия | Для чего |
| --- | --- | --- |
| CMake | 3.20+ | сборка |
| g++ или clang++ | с поддержкой C++20 | компиляция |
| gcovr | любая | измерение покрытия, необязательно |
| cppcheck | любая | статический анализ, необязательно |
| doxygen | любая | генерация документации, необязательно |

GoogleTest ставить не нужно — он подтягивается автоматически через
`FetchContent` при конфигурации.

### Сборка и запуск

```sh
cmake -S lab1 -B build
cmake --build build
./build/lab1_app
```

Собрать и сразу прогнать тесты одной строкой:

```sh
cmake -S lab1 -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

Полная пересборка с нуля. Каталоги сборки целиком генерируемые,
их можно удалять в любой момент:

```sh
rm -rf build build-cov
cmake -S lab1 -B build
cmake --build build
```

### Готовая сборка

Собирать необязательно: в разделе
[Releases](https://github.com/parislove2007/ppois-labs/releases)
опубликован исполняемый файл для Linux x86-64. Он слинкован
статически, поэтому компилятор, CMake и библиотеки для запуска
не нужны.

```sh
curl -L -o lab1_app \
  https://github.com/parislove2007/ppois-labs/releases/download/v1.0-lab1/lab1_app
chmod +x lab1_app
./lab1_app
```

Команда `chmod +x` обязательна: GitHub отдаёт файл без права
на исполнение.

### Тесты

```sh
ctest --test-dir build --output-on-failure
```

Запустить только часть тестов по имени:

```sh
ctest --test-dir build -R SetTest --output-on-failure
```

### Покрытие кода

```sh
cmake -S lab1 -B build-cov -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov
gcovr --root . --filter 'lab1/src/' --print-summary build-cov
```

То же самое одной строкой — собрать, прогнать тесты и показать
покрытие:

```sh
cmake -S lab1 -B build-cov -DENABLE_COVERAGE=ON && cmake --build build-cov && ctest --test-dir build-cov && gcovr --root . --filter 'lab1/src/' --print-summary build-cov
```

Та же проверка с порогом, которую выполняет CI — завершается
ошибкой, если покрытие строк ниже 90%:

```sh
gcovr --root . --filter 'lab1/src/' --fail-under-line 90 --print-summary build-cov
```

Подробный отчёт в HTML:

```sh
gcovr --root . --filter 'lab1/src/' --html-details coverage.html build-cov
```

### Документация

```sh
cd lab1 && doxygen Doxyfile
```

Результат — `lab1/docs/html/index.html`.

### Статический анализ

```sh
cppcheck --enable=warning,style --error-exitcode=1 \
         --suppress=missingIncludeSystem lab1/src lab1/include
```

### Проверка на утечки и неопределённое поведение

```sh
cmake -S lab1 -B build-asan -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g"
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
```

## CI

При каждом push и pull request GitHub Actions собирает проект,
прогоняет тесты, проверяет покрытие с порогом 90% и запускает
статический анализ. При push тега вида `v*` дополнительно собирается
статически слинкованный бинарный файл и публикуется в Releases.
Конфигурация — [.github/workflows/ci.yml](.github/workflows/ci.yml).