#include "Response.h"
#include "Connection.h"
#include <stdexcept>

using namespace std::literals;

std::string Response::get_status_code_msg(int code) const
{
    return std::to_string(code); // + " " + status_code_messages.at(code);
}

Response::Response(Connection& connection) : connection(connection)
{}

bool Response::send_headers(int status)
{
    if (send_state != SendState::HEADERS)
    {
        throw std::runtime_error("Headers already sent or client closed connection.");
    }
    else if (status == -1)
    {
        throw std::runtime_error("Status not set.");
    }

    std::string rep = version + " " + get_status_code_msg(status) + "\n";
    
    for (const auto& [name, content] : headers)
    {
        rep += name;
        rep += ": ";
        rep += content;
        rep += "\r\n";
    }

    rep += "\r\n";
    auto sent = connection.send_all(rep, 0);

    if (sent == rep.size())
    {
        send_state = SendState::BODY;
        return true;
    }
    else
    {
        send_state = SendState::FINISHED;
        return false;
    }
}

Response& Response::set_header(std::string text, std::string data)
{
    headers[std::move(text)] = std::move(data);
    return *this;
}


void Response::send_file(std::filesystem::path file_path)
{
    if (std::filesystem::is_regular_file(file_path))
    {
        std::uintmax_t file_size = std::filesystem::file_size(file_path);

        set_header("Content-Length", std::to_string(file_size)); // may be modified.
        // Content-Type based on extension
        if (send_headers(200))
        {
            connection.send_file(file_path);
            send_state = SendState::FINISHED;
        }
    }
    else
    {
        throw std::runtime_error("Response::send_file: "s + file_path.native() + " does not exist or is not a file.");
    }
}

void Response::send_text(int status, std::string_view text)
{
    set_header("Content-Length", std::to_string(text.size()));
    set_header("Content-Type", "text/plain");
    if (send_headers(status))
    {
        connection.send_all(text, 0);
        send_state = SendState::FINISHED;
    }
}

void Response::send_status(int status)
{
    send_headers(status);
    send_state = SendState::FINISHED;
}

void Response::send_json(int status, std::string_view json)
{
    set_header("Content-Length", std::to_string(json.size()));
    set_header("Content-Type", "application/json");
    if (send_headers(status))
    {
        connection.send_all(json, 0);
        send_state = SendState::FINISHED;
    }
}

bool Response::send_body(std::span<const char> data)
{
    if (connection.send_all(data, 0) == data.size())
    {
        return true;
    }
    else
    {
        send_state = SendState::FINISHED;
        return false;
    }
}

bool Response::write(std::span<const char> data)
{
    if (send_state == SendState::BODY)
    {
        return send_body(data);
    }
    else if (send_state == SendState::HEADERS)
    {
        return send_headers(status) and send_body(data);
    }
    else
    {
        throw std::runtime_error("Response already sent or client closed connection.");
    }
}

Response& Response::set_status(int status)
{
    this->status = status;
    return *this;
}
