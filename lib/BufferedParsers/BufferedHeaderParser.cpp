#include "BufferedHeaderParser.h"
#include "../Request.h"

ParseResult BufferedHeaderParser::title_parser(Request& req, std::string_view new_data)
{
    for (auto it = new_data.begin(); it != new_data.end(); ++it)
    {
        char byte = *it;
        if (byte == ':')
        {
            cur_header_title += { new_data.begin(), it };
            cur_parser = &BufferedHeaderParser::space_parser;
            return ParseResult::unfinished({ it + 1, new_data.end() });
        }
        else if (!(std::isalnum(byte) or byte == '-' or byte == '_'))
        {
            return ParseResult::invalid({ it, new_data.end() });
        }
    }

    cur_header_title += new_data;
    return ParseResult::unfinished("");
}

ParseResult BufferedHeaderParser::data_parser(Request& req, std::string_view new_data)
{
    for (auto it = new_data.begin(); it != new_data.end(); ++it)
    {
        char byte = *it;
        if (byte == '\n')
        {
            cur_header_data += { new_data.begin(), it };
            if (!cur_header_data.empty() and cur_header_data.back() == '\r')
            {
                cur_header_data.pop_back();
            }

            req.set_header(std::move(cur_header_title), std::move(cur_header_data));
            cur_parser = &BufferedHeaderParser::after_data_parser;
            return ParseResult::unfinished({ it + 1, new_data.end() });
        }
    }

    cur_header_data += new_data;
    return ParseResult::unfinished("");
}

ParseResult BufferedHeaderParser::space_parser(Request& req, std::string_view new_data)
{
    for (auto it = new_data.begin(); it != new_data.end(); ++it)
    {
        char byte = *it;
        if (byte != ' ')
        {
            cur_parser = &BufferedHeaderParser::data_parser;
            return ParseResult::unfinished({ it, new_data.end() });
        }
    }

    return ParseResult::unfinished("");
}

ParseResult BufferedHeaderParser::after_data_parser(Request& req, std::string_view new_data)
{
    char byte = new_data.front();

    if (cr_encountered)
    {
        if (byte == '\n')
        {
            cur_parser = nullptr;
            return ParseResult::finished(new_data.substr(1));
        }
        else
        {
            return ParseResult::invalid(new_data);
        }
    }
    else if (byte == '\r')
    {
        // Can be optimized.
        cr_encountered = true;
        return ParseResult::unfinished(new_data.substr(1));
    }
    else if (byte == '\n')
    {
        cur_parser = nullptr;
        return ParseResult::finished(new_data.substr(1));
    }
    else
    {
        cur_parser = &BufferedHeaderParser::title_parser;
        return ParseResult::unfinished(new_data);
    }
}

ParseResult BufferedHeaderParser::parse(Request& req, std::string_view new_data)
{
    if (!cur_parser) return ParseResult::finished(new_data);

    // Header: data(\r)\n
    ParseResult result = (this->*cur_parser)(req, new_data);
    auto leftover = result.get_leftover();

    while (result.is_unfinished() and !leftover.empty())
    {
        result = (this->*cur_parser)(req, leftover);
        leftover = result.get_leftover();
    }

    return result;
}
