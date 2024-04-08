#include "lib/BasicHttpServer.h"
#include "lib/Request.h"
#include "lib/Response.h"
#include "lib/FileServer.h"
#include <signal.h>
#include <iostream>

BasicHttpServer server;

void interrupt_handler(int)
{
    server.stop();
}

void handler_send_file(const Request& req, Response& res)
{
    res.send_file("public/test.html");
}

void handler_send_text(const Request& req, Response& res)
{
    std::cout << "post body: " << req.get_body() << '\n';
    std::cout << "cont type: " << req.get_header("Content-Type") << '\n';
    std::cout << "cont length: " << req.get_header("Content-Length") << '\n';
    res.send_text(200, "Hello");
}

void handler_writer(const Request& req, Response& res)
{
    using namespace std::literals;
    bool success = res.set_status(200)
        .set_header("Content-Type", "text/plain")
        .set_header("Content-Length", "5")
        .write("H"sv);

    if (!success) return;
    std::this_thread::sleep_for(2s);
    if (!res.write("el"sv)) return;
    std::this_thread::sleep_for(2s);
    res.write("lo"sv);
    std::this_thread::sleep_for(2s);
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = interrupt_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Sigaction");
        return -1;
    }

    struct sigaction sa_pipe;
    sa_pipe.sa_handler = SIG_IGN;
    sa_pipe.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa_pipe, NULL) == -1)
    {
        perror("Sigaction");
        return -1;
    }


    server.use(FileServer { "public" });
    server.get("/").end(handler_send_file);
    server.post("/hello").end(handler_send_text);
    server.post("/hello_delayed").end(handler_writer);
    server.listen("8080");
    return 0;
}
