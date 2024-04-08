#include "ConnectionPool.h"
#include "Connection.h"
#include <iostream>

ConnectionPool::ConnectionPool(int thread_pool_size, std::function<void(Connection&)> connection_handler)
    : threads(std::make_unique<std::jthread[]>(thread_pool_size)),
    connection_handler(std::move(connection_handler))
{
    for (int i = 0; i < thread_pool_size; ++i)
    {
        threads[i] = std::jthread { &ConnectionPool::thread_loop, this };
    }
}

ConnectionPool::~ConnectionPool()
{
    std::cout << "Shutting down connection pool\n";
    int connections = 0;

    {
        std::lock_guard l { queue_mutex };
        connections = connection_queue.size();
        shutdown =  true;
    }
    
    cv_connection.notify_all();

    std::cout << "Shutting down connection pool: queued connections = " << connections << "\n";
}


void ConnectionPool::thread_loop()
{
    while (true)
    {
        Connection connection;
        {
            std::unique_lock l { queue_mutex };
            cv_connection.wait(l, [this]() { return !connection_queue.empty() or shutdown; });
            if (shutdown)
            {
                return;
            }
            else if (!connection_queue.empty())
            {
                connection = std::move(connection_queue.front());
                connection_queue.pop();
            }
        }

        connection_handler(connection);
    }
}

void ConnectionPool::queue_connection(Connection&& connection)
{
    {
        std::lock_guard l { queue_mutex };
        connection_queue.push(std::move(connection));
    }

    cv_connection.notify_one();
}
