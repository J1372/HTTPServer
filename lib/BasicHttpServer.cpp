#include "BasicHttpServer.h"
#include "Connection.h"
#include "./BufferedParsers/BufferedRequestParser.h"
#include <iostream>
#include <cmath>
#include <poll.h>
#include "Request.h"
#include "Response.h"

using namespace std::chrono;
using namespace std::literals;

BasicHttpServer::BasicHttpServer()
    : connection_pool(4, [this](Connection& c) { return handle_connection(c); })
{}

void BasicHttpServer::use(Middleware middleware)
{
    global_middleware.push_back(std::move(middleware));
}

MiddlewareChain& BasicHttpServer::get(const std::string& path)
{
    return get_handlers[path]; // will create if does not exist.
}

MiddlewareChain& BasicHttpServer::post(const std::string& path)
{
    return post_handlers[path]; // will create if does not exist.
}

void BasicHttpServer::handle_request(Request& request, Response& response)
{
    for (Middleware& m : global_middleware)
    {
        if (m(request, response)) return;
    }


    // Request has a valid method. This is checked before call to here.
    auto method = request.get_method();
    auto path = request.get_path();
    auto method_path_handlers_it = method_to_path_handler_map.find(method);
    if (method_path_handlers_it == method_to_path_handler_map.end()) return;

    auto& method_path_handlers = method_path_handlers_it->second;
    auto path_handler_it = method_path_handlers.find(std::string(path));

    if (path_handler_it != method_path_handlers.end())
    {
        // path implemented
        // path-specific middleware.
        auto& path_middleware_chain = path_handler_it->second;
        path_middleware_chain.handle_req(request, response);
    }
    else
    {
        // path not implemented
        response.send_status(404);
    }
}

void BasicHttpServer::handle_connection(Connection& connection)
{
    struct pollfd pollfd { connection.get_fd(), POLLIN, 0 };

    constexpr int BUF_CAPACITY = std::pow(2, 12);
    constexpr int MAX_DATA_LENGTH = std::pow(2, 14);
    auto buf = std::make_unique_for_overwrite<char[]>(BUF_CAPACITY);
    int total_data_received = 0;
    
    BufferedRequestParser req_parser;
    Request req;
    Response res { connection };

    // Build request.
    constexpr auto timeout = 5s;
    constexpr auto poll_ms = duration_cast<milliseconds>(timeout).count();

    std::cout << "Handling connection with " << connection.get_fd() << "\n";
    std::cout << "Polling.\n";
    int receivable = poll(&pollfd, 1, poll_ms);

    while (receivable > 0)
    {
        if (pollfd.revents & POLLIN)
        {
            std::cout << "Receiving.\n";

            std::int64_t received = connection.recv(buf.get(), BUF_CAPACITY, 0);
            
            if (received <= 0) return;
            total_data_received += received;

            auto parse_result = req_parser.parse(req, { buf.get(), static_cast<std::size_t>(received) });

            if (parse_result.is_finished())
            {
                std::cout << "Receiving finished.\n";
                handle_request(req, res);
                return;
            }
            else if (parse_result.error_occurred())
            {
                res.send_status(400);
                return;
            }
            else if (total_data_received >= MAX_DATA_LENGTH)
            {
                res.send_status(413);
                return;
            }
        }

        std::cout << "Polling.\n";
        receivable = poll(&pollfd, 1, poll_ms);
    }

    // Timeout.
    if (receivable == 0)
    {
        std::cout << "Initial request receive timeout\n";
        res.send_status(408);
    }

    // Else - poll error
}


void BasicHttpServer::listen(const std::string &port)
{
    listener = { port.c_str() };
    running = true;
    
    std::cout << "Listening on port " << port << "\n";
    while (running)
    {
        std::optional<Connection> accept_result = listener.await_connection();
        if (accept_result)
        {
            Connection& connection = *accept_result;
            std::cout << "New connection with " << connection.get_address() << "\n";
            connection_pool.queue_connection(std::move(connection));
        }
        else
        {
            stop();
        }
    }
}

void BasicHttpServer::stop()
{
    running = false;
}
