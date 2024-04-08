#include "MiddlewareChain.h"
#include "Response.h"

void MiddlewareChain::unimplemented_handler(Request& req, Response& res)
{
    res.send_status(404);
}

MiddlewareChain& MiddlewareChain::add(Middleware middleware)
{
    middlewares.push_back(std::move(middleware));
    return *this;
}

void MiddlewareChain::end(RequestHandler req_endpoint)
{
    final_handler = std::move(req_endpoint);
}

void MiddlewareChain::handle_req(Request& req, Response& res)
{
    for (Middleware& m : middlewares)
    {
        if (m(req, res)) return;
    }

    // no middleware response
    
    final_handler(req, res);
}
