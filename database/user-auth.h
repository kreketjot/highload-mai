#ifndef USER_AUTH_H
#define USER_AUTH_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class UserAuth
    {
    private:
        std::string _login;
        std::string _password;

    public:
        static UserAuth fromJSON(const std::string &str);

        const std::string &getLogin() const;
        const std::string &getPassword() const;

        std::string &login();
        std::string &password();

        static void init();
        static bool auth(std::string login, std::string password);
        void saveToMysql();
        static void deleteUser(std::string login);

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif