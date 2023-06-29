#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class ChatMessage
    {
    private:
        long _id;
        std::string _sender;
        std::string _recipient;
        std::string _content;

    public:
        static ChatMessage fromJSON(const std::string &str);

        long getId() const;
        const std::string &getSender() const;
        const std::string &getRecipient() const;
        const std::string &getContent() const;

        long &id();
        std::string &sender();
        std::string &recipient();
        std::string &content();

        static void init();
        static std::vector<ChatMessage> readAllByChat(std::string user1, std::string user2);
        void saveToMysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif