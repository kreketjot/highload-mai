#include "post.h"
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

    void Post::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Post` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`title` VARCHAR(256) NOT NULL,"
                        << "`content` VARCHAR(256) NOT NULL,"
                        << "`user_id` VARCHAR(256) INT NOT NULL ,"
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

    Poco::JSON::Object::Ptr Post::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("title", _title);
        root->set("content", _content);
        root->set("user_id", _user_id);

        return root;
    }

    Post Post::fromJSON(const std::string &str)
    {
        Post post;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        post.id() = object->getValue<long>("id");
        post.title() = object->getValue<std::string>("title");
        post.content() = object->getValue<std::string>("content");
        post.user_id() = object->getValue<long>("user_id");

        return post;
    }

    std::optional<Post> Post::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Post a;
            select << "SELECT id, title, content, user_id FROM Post WHERE id=?",
                into(a._id),
                into(a._title),
                into(a._content),
                into(a._user_id),
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

    std::vector<Post> Post::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Post> result;
            Post a;
            select << "SELECT id, title, content, user_id FROM Post",
                into(a._id),
                into(a._title),
                into(a._content),
                into(a._user_id),
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

    std::vector<Post> Post::read_by_author(long userId)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Post> result;
            Post a;
            select << "SELECT id, title, content, user_id FROM Post WHERE user_id=?",
                into(a._id),
                into(a._title),
                into(a._content),
                into(a._user_id),
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

    void Post::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Post (title, content, user_id) VALUES(?, ?, ?)",
                use(_title),
                use(_content),
                use(_user_id);

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

    long Post::id() const
    {
        return _id;
    }

    long &Post::id()
    {
        return _id;
    }

    const std::string &Post::title() const
    {
        return _title;
    }

    std::string &Post::title()
    {
        return _title;
    }

    const std::string &Post::content() const
    {
        return _content;
    }

    std::string &Post::content()
    {
        return _content;
    }

    long Post::user_id() const
    {
        return _user_id;
    }

    long &Post::user_id()
    {
        return _user_id;
    }
}