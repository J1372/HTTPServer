#pragma once

#include <string>
#include "ParseResult.h"

class Request;

class BufferedReqLineParser
{

    std::string req_method;
    std::string req_path;
    std::string req_version;

    ParseResult method_parser(Request& req, std::string_view new_data);
    ParseResult path_parser(Request& req, std::string_view new_data);
    ParseResult version_parser(Request& req, std::string_view new_data);
    ParseResult (BufferedReqLineParser::*cur_parser)(Request&, std::string_view) = &BufferedReqLineParser::method_parser;

public:

    ParseResult parse(Request& req, std::string_view new_data);

};
