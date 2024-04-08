#pragma once
#include <filesystem>

class Request;
class Response;

class FileServer
{
    
    std::filesystem::path abs_serve_dir;
    bool in_directory(const std::filesystem::path& path) const;

public:

    FileServer(std::filesystem::path dir_path);

    bool operator()(const Request& req, Response& res);
};
