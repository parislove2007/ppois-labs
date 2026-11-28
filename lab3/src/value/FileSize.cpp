#include "value/FileSize.h"

#include <ostream>

#include "error/InvalidFileSizeException.h"

namespace fs
{

FileSize FileSize::fromBytes(std::int64_t bytes)
{
    if (bytes < 0)
    {
        throw InvalidFileSizeException("Size must not be negative");
    }
    return FileSize(static_cast<std::uint64_t>(bytes));
}

FileSize FileSize::fromKilobytes(std::int64_t kilobytes)
{
    if (kilobytes < 0)
    {
        throw InvalidFileSizeException("Size must not be negative");
    }
    return FileSize(static_cast<std::uint64_t>(kilobytes) * 1024ULL);
}

FileSize::FileSize() : bytes_(0)
{
}

FileSize::FileSize(std::uint64_t bytes) : bytes_(bytes)
{
}

std::uint64_t FileSize::bytes() const
{
    return bytes_;
}

bool FileSize::isZero() const
{
    return bytes_ == 0;
}

bool FileSize::fitsIn(const FileSize& limit) const
{
    return bytes_ <= limit.bytes_;
}

FileSize FileSize::operator+(const FileSize& other) const
{
    return FileSize(bytes_ + other.bytes_);
}

FileSize FileSize::operator-(const FileSize& other) const
{
    if (other.bytes_ > bytes_)
    {
        throw InvalidFileSizeException("Size underflow");
    }
    return FileSize(bytes_ - other.bytes_);
}

bool FileSize::operator<(const FileSize& other) const
{
    return bytes_ < other.bytes_;
}

bool FileSize::operator==(const FileSize& other) const
{
    return bytes_ == other.bytes_;
}

std::ostream& operator<<(std::ostream& out, const FileSize& size)
{
    static const char* kUnits[] = {"B", "KiB", "MiB", "GiB"};
    double value = static_cast<double>(size.bytes());
    int unit = 0;
    while (value >= 1024.0 && unit < 3)
    {
        value /= 1024.0;
        ++unit;
    }
    if (unit == 0)
    {
        return out << size.bytes() << " " << kUnits[0];
    }
    return out << value << " " << kUnits[unit];
}

}
