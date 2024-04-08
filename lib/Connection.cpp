#include "Connection.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <cstring>
#include <climits>
#include <fcntl.h>

using namespace std::literals;

Connection::Connection(FDescriptor&& to_move_socket, const struct sockaddr* sa)
    :   socket(std::move(to_move_socket)),
        address(init_address(sa))
{}

std::string Connection::init_address(const struct sockaddr* sa) const
{
    char buf[INET6_ADDRSTRLEN];

    if (inet_ntop(sa->sa_family, get_in_addr(sa), buf, sizeof buf) != NULL)
    {
        return buf;
    }
    else
    {
        return 0;
    }
}

const void* Connection::get_in_addr(const struct sockaddr* sa) const
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Connection::get_fd() const
{
    return socket.get_fd();
}

std::string_view Connection::get_address() const
{
    return address;
}

std::int64_t Connection::recv(char* buf, std::size_t max_recv, int flags)
{
    ssize_t received = ::recv(socket.get_fd(), buf, max_recv, flags);
    return received;
}

std::size_t Connection::send_all(std::span<const char> data, int flags) const
{
    std::size_t total_bytes_sent = 0;
    
    while (total_bytes_sent < data.size())
    {
        const char* next_send = &data[total_bytes_sent];
        std::size_t bytes_left = data.size() - total_bytes_sent;
        
        // Try to send remaining bytes.
        ssize_t bytes_sent = ::send(socket.get_fd(), next_send, bytes_left, flags);
        
        if (bytes_sent == -1) // send failed
        {
            return total_bytes_sent;
        }
        else
        {
            total_bytes_sent += bytes_sent;
        }
    }
    
    return total_bytes_sent;
}

bool Connection::send_file(std::filesystem::path file_path) const
{
    if (std::filesystem::is_regular_file(file_path))
    {
        std::uintmax_t file_size = std::filesystem::file_size(file_path);
        std::uintmax_t remaining_bytes = file_size;
        FDescriptor file = open(file_path.c_str(), O_RDONLY);
        if (file.get_fd() == -1) return false;
        
        while (remaining_bytes != 0)
        {
            // Sendfile count takes size_t arg, whose max range = SIZE_MAX <= uintmax_t.
            //  despite being size_t, sending may be impl defined if pass arg larger than SSIZE_MAX, which is < SIZE_MAX.
            size_t to_send = std::min(remaining_bytes, static_cast<std::uintmax_t>(SSIZE_MAX));
            // updates file offset
            ssize_t bytes_sent = ::sendfile(socket.get_fd(), file.get_fd(), nullptr, to_send);
            
            if (bytes_sent == -1)
            {
                return false;
            }
            else
            {
                remaining_bytes -= bytes_sent;
            }
        }

        return true;
    }
    else
    {
        throw std::runtime_error("Connection::send_file: "s + file_path.native() + " does not exist or is not a file.");
    }
}
