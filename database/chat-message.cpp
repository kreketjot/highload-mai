#include "chat-message.h"
#include "database.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void ChatMessage::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `ChatMessage` ("
                        << "`id` INT NOT NULL AUTO_INCREMENT, "
                        << "`sender` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                        << "`recipient` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                        << "`content` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                        << "PRIMARY KEY (`id`)"
                        << ");",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr ChatMessage::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("sender", _sender);
        root->set("recipient", _recipient);
        root->set("content", _content);

        return root;
    }

    ChatMessage ChatMessage::fromJSON(const std::string &str)
    {
        ChatMessage blogPost;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        blogPost.id() = object->getValue<long>("id");
        blogPost.sender() = object->getValue<std::string>("sender");
        blogPost.recipient() = object->getValue<std::string>("recipient");
        blogPost.content() = object->getValue<std::string>("content");

        return blogPost;
    }

    std::vector<ChatMessage> ChatMessage::readAllByChat(std::string user1, std::string user2)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Statement select(session);
            std::vector<ChatMessage> result;
            ChatMessage a;
            select << "SELECT id, sender, recipient, content FROM ChatMessage "
                   << "WHERE (sender=? AND recipient=?) OR (sender=? AND recipient=?) "
                   << "ORDER BY id ASC;",
                into(a._id),
                into(a._sender),
                into(a._recipient),
                into(a._content),
                use(user1),
                use(user2),
                use(user2),
                use(user1),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    void ChatMessage::saveToMysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO ChatMessage (sender, recipient, content) VALUES(?, ?, ?);",
                use(_sender),
                use(_recipient),
                use(_content);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID();",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    long ChatMessage::getId() const
    {
        return _id;
    }

    long &ChatMessage::id()
    {
        return _id;
    }

    const std::string &ChatMessage::getSender() const
    {
        return _sender;
    }

    std::string &ChatMessage::sender()
    {
        return _sender;
    }

    const std::string &ChatMessage::getRecipient() const
    {
        return _recipient;
    }

    std::string &ChatMessage::recipient()
    {
        return _recipient;
    }

    const std::string &ChatMessage::getContent() const
    {
        return _content;
    }

    std::string &ChatMessage::content()
    {
        return _content;
    }
}