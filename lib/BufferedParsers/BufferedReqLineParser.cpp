#include "BufferedReqLineParser.h"
#include "../Request.h"

ParseResult BufferedReqLineParser::method_parser(Request& req, std::string_view new_data)
{
    auto it = std::find(new_data.begin(), std::min(new_data.begin() + 10, new_data.end()), ' ');
    req_method += std::string_view{ new_data.begin(), it };

    // check valid method

    if (it != new_data.end())
    {
        cur_parser = &BufferedReqLineParser::path_parser;
        return { ParseResult::Result::UNFINISHED, { it + 1, new_data.end() }};
    }
    else if (req_method.size() >= 10)
    {
        return { ParseResult::Result::INVALID, new_data };
    }
    else
    {
        return { ParseResult::Result::UNFINISHED, "" };
    }
}

ParseResult BufferedReqLineParser::path_parser(Request& req, std::string_view new_data)
{
    auto it = std::find(new_data.begin(), std::min(new_data.begin() + 255, new_data.end()), ' ');
    req_path += std::string_view{ new_data.begin(), it };

    if (req_path[0] != '/')
    {
        return { ParseResult::Result::INVALID, new_data };
    }
    else if (it != new_data.end())
    {
        cur_parser = &BufferedReqLineParser::version_parser;
        return { ParseResult::Result::UNFINISHED, { it + 1, new_data.end() } };
    }
    else if (req_path.size() >= 255)
    {
        return { ParseResult::Result::INVALID, new_data };
    }
    else
    {
        return { ParseResult::Result::UNFINISHED, "" };
    }
}

ParseResult BufferedReqLineParser::version_parser(Request& req, std::string_view new_data)
{
    auto it = std::find(new_data.begin(), std::min(new_data.begin() + 30, new_data.end()), '\n');
    req_version += std::string_view{ new_data.begin(), it };
    
    // assert valid version parsed

    if (it != new_data.end())
    {
        if (!req_version.empty() and req_version.back() == '\r')
        {
            req_version.pop_back();
        }

        req.set_method(std::move(req_method))
            .set_path(std::move(req_path))
            .set_version(std::move(req_version));

        cur_parser = nullptr;
        return { ParseResult::Result::FINISHED, { it + 1, new_data.end() } };
    }
    else if (req_version.size() >= 30)
    {
        return { ParseResult::Result::INVALID, new_data };
    }
    else
    {
        return { ParseResult::Result::UNFINISHED, "" };
    }
}

ParseResult BufferedReqLineParser::parse(Request& req, std::string_view new_data)
{
    if (!cur_parser) return { ParseResult::Result::FINISHED, new_data };

    ParseResult result = (this->*cur_parser)(req, new_data);
    auto leftover = result.get_leftover();

    while (result.is_unfinished() and !leftover.empty())
    {
        result = (this->*cur_parser)(req, leftover);
        leftover = result.get_leftover();
    }

    return result;
}
