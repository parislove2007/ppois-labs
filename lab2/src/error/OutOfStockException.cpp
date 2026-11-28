#include "error/OutOfStockException.h"

namespace
{
std::string buildMessage(const std::string& article, int requested, int available)
{
    return "Запчасти " + article + " требуется " + std::to_string(requested)
           + " шт., доступно " + std::to_string(available) + " шт.";
}
}

namespace repair
{

OutOfStockException::OutOfStockException(const std::string& article, int requested, int available)
    : BusinessRuleException(buildMessage(article, requested, available)),
      article_(article),
      requested_(requested),
      available_(available) {}

const std::string& OutOfStockException::article() const { return article_; }

int OutOfStockException::requested() const { return requested_; }

int OutOfStockException::available() const { return available_; }

}
