#ifndef BLOG_CONFIG_H
#define BLOG_CONFIG_H

#include <string>

class BlogConfig
{
private:
    BlogConfig();
    std::string _authHost;
    std::string _authPort;
    std::string _blogPort;

public:
    static BlogConfig &get();

    std::string &authHost();
    std::string &authPort();
    std::string &blogPort();

    const std::string &getAuthHost() const;
    const std::string &getAuthPort() const;
    const std::string &getBlogPort() const;
};

#endif