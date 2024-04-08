#pragma once

#include <string>
#include "ParseResult.h"

class Request;

class BufferedHeaderParser
{

    std::string cur_header_title;
    std::string cur_header_data;

    bool cr_encountered = false;

    ParseResult title_parser(Request& req, std::string_view new_data);
    ParseResult data_parser(Request& req, std::string_view new_data);
    ParseResult space_parser(Request& req, std::string_view new_data);
    ParseResult after_data_parser(Request& req, std::string_view new_data);
    ParseResult (BufferedHeaderParser::*cur_parser)(Request&, std::string_view) = &BufferedHeaderParser::title_parser;

public:

    ParseResult parse(Request& req, std::string_view new_data);

};
