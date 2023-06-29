#include "auth-config.h"

AuthConfig::AuthConfig()
{
    auto getenv = [&](const char *p)
    { return std::getenv(p) ? std::getenv(p) : ""; };

    _authPort = getenv("AUTH_PORT");
}

AuthConfig &AuthConfig::get()
{
    static AuthConfig _instance;
    return _instance;
}

const std::string &AuthConfig::getAuthPort() const
{
    return _authPort;
}

std::string &AuthConfig::authPort()
{
    return _authPort;
}
