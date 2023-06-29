#include "message.h"
#include "../database.h"
#include "../../config/config.h"

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

    void Message::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Message` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`chat_id` VARCHAR(256) INT NOT NULL ,"
                        << "`from_user_1` VARCHAR(256) NOT NULL,"
                        << "`content` VARCHAR(256) NOT NULL,"
                        << "`status` VARCHAR(256) NOT NULL,"
                        << "PRIMARY KEY (`id`));",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Message::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("chat_id", _chat_id);
        root->set("from_user_1", _from_user_1);
        root->set("content", _content);
        root->set("status", _status);

        return root;
    }

    Message Message::fromJSON(const std::string &str)
    {
        Message message;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        message.id() = object->getValue<long>("id");
        message.chat_id() = object->getValue<long>("chat_id");
        message.from_user_1() = object->getValue<bool>("from_user_1");
        message.content() = object->getValue<std::string>("content");
        message.status() = object->getValue<std::string>("status");

        return message;
    }

    std::optional<Message> Message::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Message a;
            select << "SELECT id, chat_id, from_user_1, content, status FROM Message WHERE id=?",
                into(a._id),
                into(a._chat_id),
                into(a._from_user_1),
                into(a._content),
                into(a._status),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::vector<Message> Message::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Message> result;
            Message a;
            select << "SELECT id, chat_id, from_user_1, content, status FROM Message",
                into(a._id),
                into(a._chat_id),
                into(a._from_user_1),
                into(a._content),
                into(a._status),
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

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<Message> Message::read_by_chat(long chatId)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Message> result;
            Message a;
            select << "SELECT id, chat_id, from_user_1, content, status FROM Message WHERE user_id=?",
                into(a._id),
                into(a._chat_id),
                into(a._from_user_1),
                into(a._content),
                into(a._status),
                use(chatId),
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

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Message::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Message (chat_id, from_user_1, content, status) VALUES(?, ?, ?, ?)",
                use(_chat_id),
                use(_from_user_1),
                use(_content),
                use(_status);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
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

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long Message::id() const
    {
        return _id;
    }

    long &Message::id()
    {
        return _id;
    }

    long Message::chat_id() const
    {
        return _chat_id;
    }

    long &Message::chat_id()
    {
        return _chat_id;
    }

    bool Message::from_user_1() const
    {
        return _from_user_1;
    }

    bool &Message::from_user_1()
    {
        return _from_user_1;
    }

    const std::string &Message::content() const
    {
        return _content;
    }

    std::string &Message::content()
    {
        return _content;
    }

    const std::string &Message::status() const
    {
        return _status;
    }

    std::string &Message::status()
    {
        return _status;
    }
}