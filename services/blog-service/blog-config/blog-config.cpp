#include "blog-config.h"

BlogConfig::BlogConfig()
{
    auto getenv = [&](const char *p)
    { return std::getenv(p) ? std::getenv(p) : ""; };

    _authHost = getenv("AUTH_HOST");
    _authPort = getenv("AUTH_PORT");
    _blogPort = getenv("BLOG_PORT");
}

BlogConfig &BlogConfig::get()
{
    static BlogConfig _instance;
    return _instance;
}

const std::string &BlogConfig::getAuthHost() const
{
    return _authHost;
}

const std::string &BlogConfig::getAuthPort() const
{
    return _authPort;
}

const std::string &BlogConfig::getBlogPort() const
{
    return _blogPort;
}

std::string &BlogConfig::authHost()
{
    return _authHost;
}

std::string &BlogConfig::authPort()
{
    return _authPort;
}

std::string &BlogConfig::blogPort()
{
    return _blogPort;
}
