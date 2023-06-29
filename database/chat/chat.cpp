#include "chat.h"
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

    void Chat::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Chat` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`user_1_id` INT NOT NULL,"
                        << "`user_2_id` INT NOT NULL,"
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

    Poco::JSON::Object::Ptr Chat::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("user_2_id", _user_2_id);
        root->set("user_1_id", _user_1_id);

        return root;
    }

    Chat Chat::fromJSON(const std::string &str)
    {
        Chat chat;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        chat.id() = object->getValue<long>("id");
        chat.user_1_id() = object->getValue<long>("user_1_id");
        chat.user_2_id() = object->getValue<long>("user_2_id");

        return chat;
    }

    std::optional<Chat> Chat::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Chat a;
            select << "SELECT id, user_1_id, user_2_id FROM Chat WHERE id=?",
                into(a._id),
                into(a._user_1_id),
                into(a._user_2_id),
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

    std::vector<Chat> Chat::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Chat> result;
            Chat a;
            select << "SELECT id, user_1_id, user_2_id FROM Chat",
                into(a._id),
                into(a._user_1_id),
                into(a._user_2_id),
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

    std::vector<Chat> Chat::read_by_user_id(long userId)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Chat> result;
            Chat a;
            select << "SELECT id, user_1_id, user_2_id FROM Chat WHERE user_1_id=? OR user_2_id=?",
                into(a._id),
                into(a._user_1_id),
                into(a._user_2_id),
                use(userId),
                use(userId),
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

    void Chat::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Chat (uset_1_id,user_2_id) VALUES(?, ?)",
                use(_user_1_id),
                use(_user_2_id);

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

    long Chat::id() const
    {
        return _id;
    }

    long Chat::user_1_id() const
    {
        return _user_1_id;
    }

    long Chat::user_2_id() const
    {
        return _user_2_id;
    }

    long &Chat::id()
    {
        return _id;
    }

    long &Chat::user_1_id()
    {
        return _user_1_id;
    }

    long &Chat::user_2_id()
    {
        return _user_2_id;
    }
}