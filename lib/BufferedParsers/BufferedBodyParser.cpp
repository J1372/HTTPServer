#include "BufferedBodyParser.h"
#include "../Request.h"

BufferedBodyParser::BufferedBodyParser(std::size_t content_length)
    : content_length(content_length)
{
    body.resize(content_length); // will also set chars to NULL initially.
}

ParseResult BufferedBodyParser::parse(Request& req, std::string_view new_data)
{
    std::size_t to_go = content_length - body_bytes_read;
    if (to_go == 0)
    {
        req.set_body(std::move(body));
        return ParseResult::finished(new_data);
    }

    // clips added body if would be more than specified content length.
    std::string_view received_body = new_data.substr(0, std::min(new_data.size(), to_go));

    auto copy_it = body.begin() + body_bytes_read;
    std::copy(received_body.begin(), received_body.end(), copy_it);
    body_bytes_read += received_body.size();

    if (content_length == body_bytes_read)
    {
        req.set_body(std::move(body));
        return ParseResult::finished(new_data.substr(received_body.size()));
    }
    else
    {
        return ParseResult::unfinished(""); // all of received was body.
    }
}
