#pragma once

#include <string>
#include "ParseResult.h"

class Request;

class BufferedBodyParser
{

    std::string body;
    std::size_t content_length;
    std::size_t body_bytes_read = 0;

public:

    BufferedBodyParser(std::size_t content_length);

    ParseResult parse(Request& req, std::string_view new_data);

};
