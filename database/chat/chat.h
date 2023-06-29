#ifndef CHAT_H
#define CHAT_H

#include <string>
#include <vector>
#include <optional>
#include <Poco/JSON/Object.h>

namespace database
{
    class Chat
    {
    private:
        long _id;
        long _user_1_id;
        long _user_2_id;

    public:
        static Chat fromJSON(const std::string &str);

        long id() const;
        long user_1_id() const;
        long user_2_id() const;

        long &id();
        long &user_1_id();
        long &user_2_id();

        static void init();
        static std::optional<Chat> read_by_id(long id);
        static std::vector<Chat> read_all();
        static std::vector<Chat> read_by_user_id(long userId);
        void save_to_mysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif