#include "error/PartNotFoundException.h"

namespace
{
std::string buildMessage(const std::string& article)
{
    return "Запчасть с артикулом " + article + " не найдена";
}
}

namespace repair
{

PartNotFoundException::PartNotFoundException(const std::string& article)
    : NotFoundException(buildMessage(article)),
      article_(article) {}

const std::string& PartNotFoundException::article() const { return article_; }

}
