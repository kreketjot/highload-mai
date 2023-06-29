#include "blog-post.h"
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

    void BlogPost::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().createSession();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `BlogPost` ("
                        << "`id` INT NOT NULL AUTO_INCREMENT, "
                        << "`author` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                        << "`title` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                        << "`content` VARCHAR(2048) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL, "
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

    Poco::JSON::Object::Ptr BlogPost::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("author", _author);
        root->set("title", _title);
        root->set("content", _content);

        return root;
    }

    BlogPost BlogPost::fromJSON(const std::string &str)
    {
        BlogPost blogPost;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        blogPost.id() = object->getValue<long>("id");
        blogPost.author() = object->getValue<std::string>("author");
        blogPost.title() = object->getValue<std::string>("title");
        blogPost.content() = object->getValue<std::string>("content");

        return blogPost;
    }

    std::vector<BlogPost> BlogPost::readAllByAuthor(std::string user)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Statement select(session);
            std::vector<BlogPost> result;
            BlogPost a;
            select << "SELECT id, author, title, content FROM BlogPost "
                   << "WHERE author=? "
                   << "ORDER BY id DESC;",
                into(a._id),
                into(a._author),
                into(a._title),
                into(a._content),
                use(user),
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

    void BlogPost::saveToMysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO BlogPost (author, title, content) VALUES(?, ?, ?);",
                use(_author),
                use(_title),
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

    long BlogPost::getId() const
    {
        return _id;
    }

    long &BlogPost::id()
    {
        return _id;
    }

    const std::string &BlogPost::getAuthor() const
    {
        return _author;
    }

    std::string &BlogPost::author()
    {
        return _author;
    }

    const std::string &BlogPost::getTitle() const
    {
        return _title;
    }

    std::string &BlogPost::title()
    {
        return _title;
    }

    const std::string &BlogPost::getContent() const
    {
        return _content;
    }

    std::string &BlogPost::content()
    {
        return _content;
    }
}