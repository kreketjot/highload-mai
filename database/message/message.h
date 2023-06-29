#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include <optional>
#include <Poco/JSON/Object.h>

namespace database
{
    class Message
    {
    private:
        long _id;
        long _chat_id;
        bool _from_user_1;
        std::string _content;
        std::string _status;

    public:
        static Message fromJSON(const std::string &str);

        long id() const;
        long chat_id() const;
        bool from_user_1() const;
        const std::string &content() const;
        const std::string &status() const;

        long &id();
        long &chat_id();
        bool &from_user_1();
        std::string &content();
        std::string &status();

        static void init();
        static std::optional<Message> read_by_id(long id);
        static std::vector<Message> read_all();
        static std::vector<Message> read_by_chat(long chatId);
        void save_to_mysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif