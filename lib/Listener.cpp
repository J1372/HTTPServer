#include "Listener.h"
#include "Connection.h"
#include <netdb.h>
#include <cstring>
#include <stdexcept>

using namespace std::literals;

struct addrinfo* get_addrinfo_list(const char* port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6 
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // localhost
    
    struct addrinfo* res_list;
    
    int code = getaddrinfo(NULL, port, &hints, &res_list);
    if (code != 0)
    {
        return NULL;
    }
    
    return res_list;
}


FDescriptor start_listening(const char* port)
{
    struct addrinfo* res_list = get_addrinfo_list(port);
    
    for (struct addrinfo* it = res_list; it != NULL; it = it->ai_next)
    {
        int socket_listener = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        
        if (socket_listener == -1)
        {
            continue;
        }
        if (bind(socket_listener, it->ai_addr, it->ai_addrlen) == -1)
        {
            continue;
        }
        if (listen(socket_listener, 20) == -1)
        {
            continue;
        }
        
        freeaddrinfo(res_list);
        return socket_listener;
    }
    
    if (res_list)
        freeaddrinfo(res_list);
    
    throw std::runtime_error("Could not begin listening on port "s + port);
}

Listener::Listener(const char* port)
    : socket(start_listening(port))
{}

std::optional<Connection> Listener::await_connection() const
{
    struct sockaddr_storage new_connection;
    socklen_t addr_len = sizeof(struct sockaddr_storage);
    int conn_fd = accept(socket.get_fd(), (struct sockaddr*)&new_connection, &addr_len);
    
    if (conn_fd == -1)
    {
        return std::nullopt;
    }

    return Connection { FDescriptor { conn_fd }, (struct sockaddr*)&new_connection };
}

