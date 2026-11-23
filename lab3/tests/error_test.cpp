#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "error/ChecksumMismatchException.h"
#include "error/EmptyTrashException.h"
#include "error/EntryAlreadyExistsException.h"
#include "error/FileLockedException.h"
#include "error/FileSystemException.h"
#include "error/InvalidFileNameException.h"
#include "error/InvalidFileSizeException.h"
#include "error/NotADirectoryException.h"
#include "error/NotAFileException.h"
#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"
#include "error/QuotaExceededException.h"
#include "error/StorageFullException.h"
#include "error/VersionConflictException.h"

using namespace fs;

namespace
{

/** @brief Бросает исключение и ловит его по общей базе: проверяет и текст, и иерархию. */
template <typename Exception>
std::string raisedMessage()
{
    try
    {
        throw Exception("domain failure");
    }
    catch (const FileSystemException& error)
    {
        return error.what();
    }
    return "";
}

} // namespace

TEST(ExceptionTest, AllDeriveFromFileSystemException)
{
    EXPECT_EQ(raisedMessage<PathNotFoundException>(), "domain failure");
    EXPECT_EQ(raisedMessage<EntryAlreadyExistsException>(), "domain failure");
    EXPECT_EQ(raisedMessage<PermissionDeniedException>(), "domain failure");
    EXPECT_EQ(raisedMessage<QuotaExceededException>(), "domain failure");
    EXPECT_EQ(raisedMessage<StorageFullException>(), "domain failure");
    EXPECT_EQ(raisedMessage<InvalidFileNameException>(), "domain failure");
    EXPECT_EQ(raisedMessage<InvalidFileSizeException>(), "domain failure");
    EXPECT_EQ(raisedMessage<FileLockedException>(), "domain failure");
    EXPECT_EQ(raisedMessage<NotADirectoryException>(), "domain failure");
    EXPECT_EQ(raisedMessage<NotAFileException>(), "domain failure");
    EXPECT_EQ(raisedMessage<EmptyTrashException>(), "domain failure");
    EXPECT_EQ(raisedMessage<VersionConflictException>(), "domain failure");
    EXPECT_EQ(raisedMessage<ChecksumMismatchException>(), "domain failure");
}

TEST(ExceptionTest, BaseIsRuntimeError)
{
    const FileSystemException error("base");
    EXPECT_STREQ(error.what(), "base");
    const std::runtime_error& asStd = error;
    EXPECT_STREQ(asStd.what(), "base");
}
