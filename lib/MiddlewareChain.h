#pragma once
#include "ReqHandlerTypes.h"
#include <vector>

class MiddlewareChain
{

    static void unimplemented_handler(Request& req, Response& res);

    std::vector<Middleware> middlewares;
    RequestHandler final_handler = MiddlewareChain::unimplemented_handler;

public:

    MiddlewareChain& add(Middleware middleware);
    
    void end(RequestHandler req_endpoint);

    void handle_req(Request& req, Response& res);

};
