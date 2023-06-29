#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H

#include <string>

class AuthConfig
{
private:
    AuthConfig();
    std::string _authPort;

public:
    static AuthConfig &get();

    std::string &authPort();

    const std::string &getAuthPort() const;
};

#endif