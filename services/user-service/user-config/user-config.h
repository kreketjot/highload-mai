#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include <string>

class UserConfig
{
private:
    UserConfig();
    std::string _authHost;
    std::string _authPort;
    std::string _userPort;

public:
    static UserConfig &get();

    std::string &authHost();
    std::string &authPort();
    std::string &userPort();

    const std::string &getAuthHost() const;
    const std::string &getAuthPort() const;
    const std::string &getUserPort() const;
};

#endif