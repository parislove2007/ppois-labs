#pragma once

#include <iosfwd>
#include <string>

#include "service/FileManager.h"
#include "service/ShareService.h"
#include "user/Administrator.h"
#include "user/Guest.h"
#include "user/RegularUser.h"
#include "value/Clock.h"

namespace fs
{

/**
 * @brief Консольный интерфейс файлового менеджера.
 *
 * Только ввод-вывод: вся логика остаётся в FileManager и доменных
 * классах. Команды читаются построчно, ошибки домена печатаются.
 */
class ConsoleMenu
{
public:
    ConsoleMenu(std::istream& in, std::ostream& out, FileManager& manager);

    /** @brief Главный цикл: читает команды до «exit». */
    void run();

private:
    void handleCommand(const std::string& line);
    void showHelp();
    void doTree();
    void doCreateFile(const std::string& path, const std::string& content);
    void doRead(const std::string& path);
    void doRemove(const std::string& path);
    void doSearch(const std::string& needle);
    void doVersions(const std::string& path);
    void doJournal();
    void doQuota(const std::string& login);
    void doShare(const std::string& path);
    void doResolveShare(const std::string& token);
    void switchUser(const std::string& role);

    std::istream& in_;
    std::ostream& out_;
    FileManager& manager_;
    Administrator admin_;
    RegularUser user_;
    Guest guest_;
    User* current_;
    Clock clock_;
    ShareService shares_;
};

}
