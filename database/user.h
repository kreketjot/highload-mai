#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class User
    {
    private:
        std::string _login;
        std::string _firstName;
        std::string _lastName;
        std::string _email;
        std::string _title;

    public:
        static User fromJSON(const std::string &str);

        const std::string &getLogin() const;
        const std::string &getFirstName() const;
        const std::string &getLastName() const;
        const std::string &getEmail() const;
        const std::string &getTitle() const;

        std::string &login();
        std::string &firstName();
        std::string &lastName();
        std::string &email();
        std::string &title();

        static void init();
        static std::optional<User> readByLogin(std::string login);
        static std::vector<User> readAll();
        static std::vector<User> search(std::string firstName, std::string lastName);
        void saveToMysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif