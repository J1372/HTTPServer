#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include "StringConvert.h"

using HttpHeaders = std::unordered_map<std::string, std::string>;

class Request
{
    
    std::string method;
    std::string path;
    HttpHeaders headers;
    std::string body;
    std::string version;
    
public:

    Request() = default;
    Request(const std::string& type, const std::string& uri, const std::string& version, HttpHeaders&& headers, std::string&& body);

    // Return header value as a string if exists otherwise empty string.
    std::string_view get_header(const std::string& header) const;
    
    // Return header value converted to T if it exists and is convertible to T, otherwise nullopt.
    template <typename T>
    std::optional<T> get_header(const std::string& header) const
    {
        return StringConvert::to<T>(get_header(header));
    }

    std::string_view get_body() const;
    std::string_view get_method() const;
    std::string_view get_path() const;
    std::string_view get_version() const;

    std::string get_string_rep() const; // could string_view and lazy evaluate string.

    Request& set_method(std::string method);
    Request& set_path(std::string uri);
    Request& set_version(std::string version);
    Request& set_header(std::string method, std::string data);

    void set_body(std::string body);

};
