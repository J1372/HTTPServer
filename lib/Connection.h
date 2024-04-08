#pragma once

#include "FDescriptor.h"
#include <netinet/in.h>
#include <string>
#include <span>
#include <filesystem>


// TCP connection and metadata.
class Connection
{

    FDescriptor socket;
    std::string address;

    const void* get_in_addr(const struct sockaddr* sa) const;
    std::string init_address(const struct sockaddr* sa) const;

public:
    
    Connection() = default;
    Connection(FDescriptor&& to_move_socket, const struct sockaddr* sa);

    int get_fd() const;
    std::string_view get_address() const;

    std::int64_t recv(char* buf, std::size_t max_recv, int flags);
    std::size_t send_all(std::span<const char> data, int flags) const;
    bool send_file(std::filesystem::path file_path) const;

};
