#pragma once

#include <string_view>
#include <charconv>
#include <optional>

namespace StringConvert
{
    // Parse string view into a number if possible.
    template<typename T>
    std::optional<T> to(std::string_view sv)
    {
        T val;
        if (std::from_chars(sv.data(), sv.data() + sv.size(), val).ec == std::errc{})
        {
            return val;
        }
        else
        {
            return std::nullopt;
        }
    }
}
