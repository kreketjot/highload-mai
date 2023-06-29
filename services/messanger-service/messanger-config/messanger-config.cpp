#include "messanger-config.h"

MessangerConfig::MessangerConfig()
{
    auto getenv = [&](const char *p)
    { return std::getenv(p) ? std::getenv(p) : ""; };

    _authHost = getenv("AUTH_HOST");
    _authPort = getenv("AUTH_PORT");
    _messangerPort = getenv("MESSANGER_PORT");
}

MessangerConfig &MessangerConfig::get()
{
    static MessangerConfig _instance;
    return _instance;
}

const std::string &MessangerConfig::getAuthHost() const
{
    return _authHost;
}

const std::string &MessangerConfig::getAuthPort() const
{
    return _authPort;
}

const std::string &MessangerConfig::getMessangerPort() const
{
    return _messangerPort;
}

std::string &MessangerConfig::authHost()
{
    return _authHost;
}

std::string &MessangerConfig::authPort()
{
    return _authPort;
}

std::string &MessangerConfig::messangerPort()
{
    return _messangerPort;
}
