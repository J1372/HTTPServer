#pragma once

#include <variant>
#include "ParseResult.h"
#include "BufferedReqLineParser.h"
#include "BufferedHeaderParser.h"
#include "BufferedBodyParser.h"

class Request;

class BufferedRequestParser
{
    using ParserType = std::variant<
        std::monostate,
        BufferedReqLineParser,
        BufferedHeaderParser,
        BufferedBodyParser
    >;

    ParserType current_parser { BufferedReqLineParser{} };

    template <typename T>
    constexpr void transition_state(Request& req);

public:

    ParseResult parse(Request& req, std::string_view new_data);

};
