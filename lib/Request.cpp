#include "Request.h"
#include <cassert>

Request::Request(const std::string& type, const std::string& uri, const std::string& version, HttpHeaders&& headers, std::string&& body)
    : method(type), path(uri), headers(std::move(headers)), body(std::move(body)), version(version)
{}

std::string_view Request::get_header(const std::string& header) const
{
    auto it = headers.find(header);

    if (it != headers.end())
    {
        return it->second;
    }
    else
    {
        return "";
    }
};

std::string_view Request::get_body() const { return body; }; 
std::string_view Request::get_method() const { return method; }; 
std::string_view Request::get_path() const { return path; }; 

std::string Request::get_string_rep() const
{
    std::string rep = method + " " + path + " " + version + "\r\n";
    
    for (const auto& [name, content] : headers)
    {
        rep += name;
        rep += ": ";
        rep += content;
        rep += "\r\n";
    }

    rep += "\r\n";
    rep += body;
    return rep;
}

Request& Request::set_method(std::string method)
{ 
    this->method = std::move(method);
    return *this;
}
Request& Request::set_path(std::string path)
{
    this->path = std::move(path);
    return *this;
}
Request& Request::set_version(std::string version)
{ 
    this->version = std::move(version);
    return *this;
}

Request& Request::set_header(std::string method, std::string data)
{ 
    headers[std::move(method)] = std::move(data);
    return *this;
}

void Request::set_body(std::string body)
{ 
    this->body = std::move(body);
}

std::string_view Request::get_version() const
{
    return version;
}
