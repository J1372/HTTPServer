#include "FileServer.h"
#include "Request.h"
#include "Response.h"
#include <fstream>

using namespace std::literals;

FileServer::FileServer(std::filesystem::path dir_path)
    : abs_serve_dir(std::filesystem::canonical(dir_path))
{
    if (!std::filesystem::is_directory(abs_serve_dir))
    {
        throw std::runtime_error(abs_serve_dir.string() + " is not a directory.");
    }
}

bool FileServer::in_directory(const std::filesystem::path& path) const
{
    // path is absolute.
    auto [mismatch, _] = std::mismatch(abs_serve_dir.begin(), abs_serve_dir.end(), path.begin());
    return mismatch == abs_serve_dir.end();
}

bool FileServer::operator()(const Request& req, Response& res)
{
    if (req.get_method() != "GET") return false;

    auto resource = req.get_path();
    auto abs_res_path = abs_serve_dir;
    abs_res_path += resource;
    abs_res_path = std::filesystem::weakly_canonical(abs_res_path);

    if (in_directory(abs_res_path) and std::filesystem::is_regular_file(abs_res_path))
    {
        res.send_file(abs_res_path);
        return true;
    }
    else
    {
        return false;
    }
}
