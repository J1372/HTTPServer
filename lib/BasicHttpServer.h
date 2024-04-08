#pragma once

#include "Listener.h"
#include "ConnectionPool.h"
#include "ReqHandlerTypes.h"
#include "MiddlewareChain.h"
#include <unordered_map>
#include <string>

class Connection;

class BasicHttpServer
{

    ConnectionPool connection_pool;
    bool running = false;

    void handle_connection(Connection& connection);
    void handle_request(Request& request, Response& response);

    using PathHandlerMap = std::unordered_map<std::string, MiddlewareChain>;
    std::vector<Middleware> global_middleware;
    PathHandlerMap get_handlers;
    PathHandlerMap post_handlers;
    std::unordered_map<std::string_view, PathHandlerMap&> method_to_path_handler_map
    {
        { "GET", get_handlers},
        { "POST", post_handlers},
    };
    
    Listener listener;

public:

    BasicHttpServer();

    void listen(const std::string& port);

    void use(Middleware middleware);
    MiddlewareChain& get(const std::string& path);
    MiddlewareChain& post(const std::string& path);

    void stop();

};
