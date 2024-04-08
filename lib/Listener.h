#pragma once

#include "FDescriptor.h"
#include <optional>

class Connection;

class Listener
{

    FDescriptor socket;

public:

    Listener() = default;
    Listener(const char* port);
    std::optional<Connection> await_connection() const;

};
