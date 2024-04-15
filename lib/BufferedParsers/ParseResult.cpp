#include "ParseResult.h"


ParseResult::ParseResult(Result result, std::string_view leftover)
    : result(result), leftover(leftover)
{}

ParseResult ParseResult::unfinished(std::string_view leftover)
{
    return { Result::UNFINISHED, leftover };
}

ParseResult ParseResult::finished(std::string_view leftover)
{
    return { Result::FINISHED, leftover };
}

ParseResult ParseResult::invalid(std::string_view leftover)
{
    return { Result::INVALID, leftover };
}

bool ParseResult::is_finished() const
{
    return result == Result::FINISHED;
}

bool ParseResult::is_unfinished() const
{
    return result == Result::UNFINISHED;
}

bool ParseResult::error_occurred() const
{
    return result == Result::INVALID;
}

std::string_view ParseResult::get_leftover() const
{
    return leftover;
}
