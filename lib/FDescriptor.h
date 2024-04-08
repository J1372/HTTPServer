#pragma once

// RAII file descriptor. Closes socket / file on destruction.
class FDescriptor
{

    int fd = -1;

public:

    FDescriptor() = default;
    FDescriptor(int fd);

    FDescriptor(const FDescriptor& copy) = delete;
    FDescriptor& operator=(const FDescriptor& copy) = delete;
    FDescriptor(FDescriptor&& move);
    FDescriptor& operator=(FDescriptor&& move);
    ~FDescriptor();

    int get_fd() const
    {
        return fd;
    }
    
    void close();
};
