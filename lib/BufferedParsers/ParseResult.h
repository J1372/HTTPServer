#pragma once

#include <string_view>

class ParseResult
{
public:

    enum class Result
    {
        UNFINISHED,
        FINISHED,
        INVALID
    };

    ParseResult(Result result, std::string_view leftover);

    bool is_finished() const;
    bool is_unfinished() const;
    bool error_occurred() const;

    std::string_view get_leftover() const;

private:

    Result result;
    std::string_view leftover;

};

