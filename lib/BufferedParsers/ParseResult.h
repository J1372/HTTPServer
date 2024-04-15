#pragma once

#include <string_view>

class ParseResult
{

    enum class Result
    {
        UNFINISHED,
        FINISHED,
        INVALID
    };

    Result result;
    std::string_view leftover;

    ParseResult(Result result, std::string_view leftover);

public:

    static ParseResult unfinished(std::string_view leftover);
    static ParseResult finished(std::string_view leftover);
    static ParseResult invalid(std::string_view leftover);

    bool is_finished() const;
    bool is_unfinished() const;
    bool error_occurred() const;

    std::string_view get_leftover() const;
    
};

