#include "user-config.h"

UserConfig::UserConfig()
{
    auto getenv = [&](const char *p)
    { return std::getenv(p) ? std::getenv(p) : ""; };

    _authHost = getenv("AUTH_HOST");
    _authPort = getenv("AUTH_PORT");
    _userPort = getenv("USER_PORT");
}

UserConfig &UserConfig::get()
{
    static UserConfig _instance;
    return _instance;
}

const std::string &UserConfig::getAuthHost() const
{
    return _authHost;
}

const std::string &UserConfig::getAuthPort() const
{
    return _authPort;
}

const std::string &UserConfig::getUserPort() const
{
    return _userPort;
}

std::string &UserConfig::authHost()
{
    return _authHost;
}

std::string &UserConfig::authPort()
{
    return _authPort;
}

std::string &UserConfig::userPort()
{
    return _userPort;
}
