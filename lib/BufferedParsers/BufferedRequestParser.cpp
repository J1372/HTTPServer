#include "BufferedRequestParser.h"
#include "../Request.h"
#include <type_traits>

template <typename T>
constexpr void BufferedRequestParser::transition_state(Request& req)
{
    if constexpr(std::is_same_v<T, BufferedReqLineParser>)
    {
        current_parser.emplace<BufferedHeaderParser>();
    }
    else if constexpr(std::is_same_v<T, BufferedHeaderParser>)
    {
        // Transition to body if content length exists and > 0
        // Otw, null state.
        auto content_length = req.get_header<std::size_t>("Content-Length");
        if (content_length and *content_length != 0)
        {
            current_parser.emplace<BufferedBodyParser>(*content_length);
        }
        else
        {
            current_parser.emplace<std::monostate>();
        }
    }
    else if constexpr(std::is_same_v<T, BufferedBodyParser>)
    {
        current_parser.emplace<std::monostate>();
    }
}

ParseResult BufferedRequestParser::parse(Request& req, std::string_view new_data)
{
    if (std::holds_alternative<std::monostate>(current_parser))
    {
        return ParseResult::finished(new_data);
    }

    ParseResult result = ParseResult::unfinished(new_data);
    std::string_view leftover = result.get_leftover();

    while (!result.error_occurred() and !leftover.empty() and !std::holds_alternative<std::monostate>(current_parser))
    {
        result = std::visit([this, &req, leftover](auto&& parser) -> ParseResult
        {
            using T = std::decay_t<decltype(parser)>;
            if constexpr (std::is_same_v<T, std::monostate>)
            {
                // should never occur.
                return ParseResult::invalid(leftover);
            }
            else
            {
                ParseResult result = parser.parse(req, leftover);
                if (result.is_finished())
                {
                    // Transition parse state.
                    transition_state<T>(req);
                }
                
                return result;
            }

        }, current_parser);

        leftover = result.get_leftover();
    }

    if (result.is_finished() and !std::holds_alternative<std::monostate>(current_parser))
    {
        // inner parse result is finished - but request parsing is not.
        return ParseResult::unfinished(leftover);
    }
    else
    {
        // return inner parse result.
        return result;
    }
}
