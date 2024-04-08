#include "FDescriptor.h"
#include <unistd.h>
#include <sys/socket.h>

FDescriptor::FDescriptor(int fd) : fd(fd)
{}

FDescriptor::FDescriptor(FDescriptor&& move) : fd(move.fd)
{
    move.fd = -1;
}

FDescriptor& FDescriptor::operator=(FDescriptor&& move)
{
    close();
    fd = move.fd;
    move.fd = -1;

    return *this;
}

FDescriptor::~FDescriptor()
{
    close();
}

void FDescriptor::close()
{
    if (fd != -1)
    {
        ::close(fd);
        fd = -1;
    }
}
