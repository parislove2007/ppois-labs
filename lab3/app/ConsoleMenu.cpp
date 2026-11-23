#include "ConsoleMenu.h"

#include <iostream>
#include <sstream>

#include "entry/File.h"
#include "error/FileSystemException.h"
#include "search/NameFilter.h"
#include "service/ReportService.h"
#include "value/Path.h"

namespace fs
{

namespace
{

std::string secondWord(const std::string& line)
{
    std::istringstream parser(line);
    std::string command;
    std::string argument;
    parser >> command >> argument;
    return argument;
}

std::string restAfterSecond(const std::string& line)
{
    std::istringstream parser(line);
    std::string command;
    std::string argument;
    std::string rest;
    parser >> command >> argument;
    std::getline(parser, rest);
    if (!rest.empty() && rest.front() == ' ')
    {
        rest.erase(0, 1);
    }
    return rest;
}

} // namespace

ConsoleMenu::ConsoleMenu(std::istream& in, std::ostream& out, FileManager& manager)
    : in_(in),
      out_(out),
      manager_(manager),
      admin_("admin", "Administrator", Timestamp::of(0)),
      user_("ivan", "Ivan Ivanov", Timestamp::of(0)),
      guest_("guest", "Guest", Timestamp::of(0)),
      current_(&user_),
      clock_(1)
{
}

void ConsoleMenu::run()
{
    showHelp();
    std::string line;
    out_ << "> ";
    while (std::getline(in_, line))
    {
        if (line == "exit")
        {
            return;
        }
        try
        {
            handleCommand(line);
        }
        catch (const FileSystemException& error)
        {
            out_ << "error: " << error.what() << "\n";
        }
        out_ << "> ";
    }
}

void ConsoleMenu::showHelp()
{
    out_ << "Commands:\n"
         << "  help | user admin|ivan|guest | tree | summary\n"
         << "  mkdir <path> | touch <path> <content> | read <path>\n"
         << "  write <path> <content> | rm <path> | restore <index>\n"
         << "  mv <from> <to> | cp <from> <to> | ren <path> <new name>\n"
         << "  tag <path> <label> | commit <path> <content> | versions <path>\n"
         << "  rollback <path> <number> | search <substring>\n"
         << "  quota <login> | backup | restore-backup | journal | exit\n";
}

void ConsoleMenu::handleCommand(const std::string& line)
{
    const std::string argument = secondWord(line);
    const std::string rest = restAfterSecond(line);
    const Timestamp at = Timestamp::of(clock_++);

    if (line == "help")
    {
        showHelp();
    }
    else if (line.rfind("user ", 0) == 0)
    {
        switchUser(argument);
    }
    else if (line == "tree")
    {
        doTree();
    }
    else if (line == "summary")
    {
        out_ << ReportService().summaryLine(manager_.storage()) << "\n";
    }
    else if (line.rfind("mkdir ", 0) == 0)
    {
        manager_.createDirectory(Path::parse(argument), *current_, at);
        out_ << "created directory " << argument << "\n";
    }
    else if (line.rfind("touch ", 0) == 0)
    {
        doCreateFile(argument, rest);
    }
    else if (line.rfind("write ", 0) == 0)
    {
        manager_.write(Path::parse(argument), rest, at);
        out_ << "written " << argument << "\n";
    }
    else if (line.rfind("read ", 0) == 0)
    {
        doRead(argument);
    }
    else if (line.rfind("rm ", 0) == 0)
    {
        doRemove(argument);
    }
    else if (line.rfind("restore ", 0) == 0)
    {
        manager_.restoreFromTrash(std::stoul(argument), at);
        out_ << "restored\n";
    }
    else if (line.rfind("mv ", 0) == 0)
    {
        const std::string to = restAfterSecond("mv " + argument);
        manager_.move(Path::parse(argument), Path::parse(to), at);
        out_ << "moved\n";
    }
    else if (line.rfind("cp ", 0) == 0)
    {
        const std::string to = restAfterSecond("cp " + argument);
        manager_.copyFile(Path::parse(argument), Path::parse(to), at);
        out_ << "copied\n";
    }
    else if (line.rfind("ren ", 0) == 0)
    {
        manager_.rename(Path::parse(argument), rest, at);
        out_ << "renamed\n";
    }
    else if (line.rfind("tag ", 0) == 0)
    {
        manager_.tag(Path::parse(argument), Tag(rest));
        out_ << "tagged\n";
    }
    else if (line.rfind("commit ", 0) == 0)
    {
        manager_.saveVersion(Path::parse(argument), rest, at);
        out_ << "version saved\n";
    }
    else if (line.rfind("versions ", 0) == 0)
    {
        doVersions(argument);
    }
    else if (line.rfind("rollback ", 0) == 0)
    {
        manager_.rollback(Path::parse(argument), std::stoi(rest), at);
        out_ << "rolled back\n";
    }
    else if (line.rfind("search ", 0) == 0)
    {
        doSearch(argument);
    }
    else if (line.rfind("quota ", 0) == 0)
    {
        doQuota(argument);
    }
    else if (line == "backup")
    {
        manager_.makeBackup();
        out_ << "backup created\n";
    }
    else if (line == "restore-backup")
    {
        out_ << "restored " << manager_.restoreBackup(at) << " files\n";
    }
    else if (line == "journal")
    {
        doJournal();
    }
    else
    {
        out_ << "unknown command, try help\n";
    }
}

void ConsoleMenu::doTree()
{
    out_ << ReportService().treeText(manager_.storage());
}

void ConsoleMenu::doCreateFile(const std::string& path, const std::string& content)
{
    manager_.createFile(Path::parse(path), content, *current_, Timestamp::of(clock_++));
    out_ << "created " << path << "\n";
}

void ConsoleMenu::doRead(const std::string& path)
{
    const FileSystemEntry& entry = manager_.storage().resolve(Path::parse(path));
    out_ << dynamic_cast<const File&>(entry).read() << "\n";
}

void ConsoleMenu::doRemove(const std::string& path)
{
    manager_.remove(Path::parse(path), Timestamp::of(clock_++));
    out_ << "moved to trash\n";
}

void ConsoleMenu::doSearch(const std::string& needle)
{
    for (const Path& found : manager_.search(NameFilter(needle)))
    {
        out_ << found << "\n";
    }
}

void ConsoleMenu::doVersions(const std::string& path)
{
    VersionHistory& history = manager_.versionHistoryFor(Path::parse(path));
    if (history.count() == 0)
    {
        out_ << "no versions yet\n";
        return;
    }
    out_ << "versions: " << history.count() << ", latest: " << history.latest().content()
         << "\n";
}

void ConsoleMenu::doJournal()
{
    out_ << manager_.journal().historyText();
}

void ConsoleMenu::doQuota(const std::string& login)
{
    out_ << "used " << manager_.quotas().usedOf(login).bytes() << " of "
         << manager_.quotas().quotaOf(login).bytes() << " bytes\n";
}

void ConsoleMenu::switchUser(const std::string& role)
{
    if (role == "admin")
    {
        current_ = &admin_;
    }
    else if (role == "ivan")
    {
        current_ = &user_;
    }
    else if (role == "guest")
    {
        current_ = &guest_;
    }
    else
    {
        out_ << "unknown user\n";
        return;
    }
    out_ << "current user: " << current_->login() << "\n";
}

}
