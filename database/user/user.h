#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <optional>
#include <Poco/JSON/Object.h>

namespace database
{
    class User
    {
    private:
        long _id;
        std::string _first_name;
        std::string _last_name;
        std::string _email;
        std::string _title;
        std::string _login;
        std::string _password;

    public:
        static User fromJSON(const std::string &str);

        long id() const;
        const std::string &first_name() const;
        const std::string &last_name() const;
        const std::string &email() const;
        const std::string &title() const;
        const std::string &login() const;
        const std::string &password() const;

        long &id();
        std::string &first_name();
        std::string &last_name();
        std::string &email();
        std::string &title();
        std::string &login();
        std::string &password();

        static void init();
        static std::optional<User> read_by_id(long id);
        static std::optional<long> auth(const std::string &login, const std::string &password);
        static std::vector<User> read_all();
        static std::vector<User> search(const std::string &first_name, const std::string &last_name);
        void save_to_mysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif