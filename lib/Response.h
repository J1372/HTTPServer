#pragma once

#include <string>
#include <span>
#include <string_view>
#include <unordered_map>
#include <filesystem>

class Connection;

class Response
{

    Connection& connection;
    std::unordered_map<std::string, std::string> headers;
    int status = -1;

    std::string version = "HTTP/1.1";

    // what is being sent to client
    enum class SendState
    {
        HEADERS,
        BODY,
        FINISHED
    } send_state = SendState::HEADERS;

    std::string get_status_code_msg(int code) const;

    bool send_headers(int status);
    bool send_body(std::span<const char> data);

public:

    Response(Connection& connection);

    // These methods close the response
    void send_file(std::filesystem::path file_path);
    void send_text(int status, std::string_view body);
    void send_json(int status, std::string_view body);
    void send_status(int status);

    // Streaming methods.
    Response& set_header(std::string text, std::string data);
    Response& set_status(int status);
    // String literals will write null terminator too ...
    bool write(std::span<const char> data);

};
