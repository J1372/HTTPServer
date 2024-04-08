#pragma once

#include <memory>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>

class Connection;

class ConnectionPool
{

    std::unique_ptr<std::jthread[]> threads;

    std::queue<Connection> connection_queue;
    std::mutex queue_mutex;
    std::condition_variable cv_connection;

    std::function<void(Connection&)> connection_handler;

    bool shutdown = false;

    void thread_loop();

public:

    ConnectionPool(int thread_pool_size, std::function<void(Connection&)> connection_handler);

    ConnectionPool(ConnectionPool&& move) = delete;
    ConnectionPool(const ConnectionPool& copy) = delete;
    ConnectionPool& operator=(ConnectionPool&& move) = delete;
    ConnectionPool& operator=(const ConnectionPool& copy) = delete;
    ~ConnectionPool();

    void queue_connection(Connection&& connection);


};
