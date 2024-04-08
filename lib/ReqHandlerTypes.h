#pragma once

#include <functional>

class Request;
class Response;

using RequestHandler = std::function<void(Request&, Response&)>;
using Middleware = std::function<bool(Request&, Response&)>;
