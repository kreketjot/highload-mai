#ifndef MESSANGER_CONFIG_H
#define MESSANGER_CONFIG_H

#include <string>

class MessangerConfig
{
private:
    MessangerConfig();
    std::string _authHost;
    std::string _authPort;
    std::string _messangerPort;

public:
    static MessangerConfig &get();

    std::string &authHost();
    std::string &authPort();
    std::string &messangerPort();

    const std::string &getAuthHost() const;
    const std::string &getAuthPort() const;
    const std::string &getMessangerPort() const;
};

#endif