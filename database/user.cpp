#include "user.h"
#include "database.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <algorithm>
#include <future>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void User::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            for (const std::string &hint : database::Database::getAllHints())
            {
                Statement create_stmt(session);
                create_stmt << "CREATE TABLE IF NOT EXISTS `User` ("
                            << "`login` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                            << "`first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                            << "`last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL, "
                            << "`email` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL, "
                            << "`title` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL, "
                            << "PRIMARY KEY (`login`), KEY `fn` (`first_name`), KEY `ln` (`last_name`)"
                            << "); "
                            << hint,
                    now;

                std::cout << create_stmt.toString() << std::endl;
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("login", _login);
        root->set("firstName", _firstName);
        root->set("lastName", _lastName);
        root->set("email", _email);
        root->set("title", _title);

        return root;
    }

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.login() = object->getValue<std::string>("login");
        user.firstName() = object->getValue<std::string>("firstName");
        user.lastName() = object->getValue<std::string>("lastName");
        user.email() = object->getValue<std::string>("email");
        user.title() = object->getValue<std::string>("title");

        return user;
    }

    std::optional<User> User::readByLogin(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement select(session);
            User a;
            std::string shardingHint = database::Database::shardingHint(login);
            std::cout << "shardingHint: " << shardingHint << std::endl;
            select
                << "SELECT login, first_name, last_name, email, title FROM User "
                << "WHERE login=?; "
                << shardingHint,
                into(a._login),
                into(a._firstName),
                into(a._lastName),
                into(a._email),
                into(a._title),
                use(login),
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
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
        }
        return {};
    }

    std::vector<User> User::readAll()
    {
        try
        {
            std::vector<std::string> hints = database::Database::getAllHints();
            std::vector<std::future<std::vector<User>>> futures;

            for (const std::string &hint : hints)
            {
                auto handle = std::async(
                    std::launch::async, [hint]() -> std::vector<User>
                    {
                    Poco::Data::Session session = database::Database::get().createSession();
                    Statement select(session);
                    std::vector<User> result;
                    // User a;
                    // select << "SELECT login, first_name, last_name, email, title FROM User; "
                    //     << hint,
                    //     into(a._login),
                    //     into(a._firstName),
                    //     into(a._lastName),
                    //     into(a._email),
                    //     into(a._title),
                    //     range(0, 1); //  iterate over result set one row at a time

                    // std::cout << hint << std::endl;

                    // while (!select.done())
                    // {
                    //     if (select.execute()) {
                    //         std::cout << "here" << std::endl;
                    //         // std::cout << "selected: " << a.getLogin() << std::endl;
                    //         result.push_back(a);
                    //     }
                    // }

                    std::string selectQuery = "SELECT login, first_name, last_name, email, title FROM User; " + hint;
                    select << selectQuery;
                    select.execute();
                    Poco::Data::RecordSet recordSet(select);

                    bool more = recordSet.moveFirst();
                    while (more)
                    {
                        for (auto &x : recordSet)
                        {
                            std::cout << "x: " << x << std::endl;
                        }
                        User user;
                        user.login() = recordSet[0].convert<std::string>();
                        user.firstName() = recordSet[1].convert<std::string>();
                        user.lastName() = recordSet[2].convert<std::string>();
                        user.email() = recordSet[3].convert<std::string>();
                        user.title() = recordSet[4].convert<std::string>();
                        result.push_back(user);
                        more = recordSet.moveNext();
                    }

                    return result; });

                futures.emplace_back(std::move(handle));
            }

            std::vector<User> users;

            // reduce
            for (std::future<std::vector<User>> &future : futures)
            {
                std::vector<User> usersFromShard = future.get();
                std::copy(std::begin(usersFromShard), std::end(usersFromShard), std::back_inserter(users));
            }

            return users;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    std::vector<User> User::search(std::string firstName, std::string lastName)
    {
        try
        {
            std::vector<std::string> hints = database::Database::getAllHints();
            std::vector<std::future<std::vector<User>>> futures;

            for (const std::string &hint : hints)
            {
                auto handle = std::async(std::launch::async, [hint, firstName, lastName]() -> std::vector<User>
                                         {
                    Poco::Data::Session session = database::Database::get().createSession();
                    Statement select(session);
                    std::vector<User> result;
                    User a;
                    std::string fn = firstName + "%";
                    std::string ln = lastName + "%";
                    select << "SELECT login, first_name, last_name, email, title FROM User "
                           << "WHERE first_name LIKE ? AND last_name LIKE ?; "
                           << hint,
                        into(a._login),
                        into(a._firstName),
                        into(a._lastName),
                        into(a._email),
                        into(a._title),
                        use(fn),
                        use(ln),
                        range(0, 1); //  iterate over result set one row at a time

                    while (!select.done())
                    {
                        if (select.execute())
                            result.push_back(a);
                    }
                    return result; });

                futures.emplace_back(std::move(handle));
            }
            std::vector<User> users;

            // reduce
            for (std::future<std::vector<User>> &future : futures)
            {
                std::vector<User> usersFromShard = future.get();
                std::copy(std::begin(usersFromShard), std::end(usersFromShard), std::back_inserter(users));
            }

            return users;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    void User::saveToMysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement insert(session);

            std::string hint = database::Database::shardingHint(_login);

            insert
                << "INSERT INTO User (login, first_name, last_name, email, title) VALUES(?, ?, ?, ?, ?); "
                << hint,
                use(_login),
                use(_firstName),
                use(_lastName),
                use(_email),
                use(_title);

            insert.execute();
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    const std::string &User::getLogin() const
    {
        return _login;
    }

    std::string &User::login()
    {
        return _login;
    }

    const std::string &User::getFirstName() const
    {
        return _firstName;
    }

    std::string &User::firstName()
    {
        return _firstName;
    }

    const std::string &User::getLastName() const
    {
        return _lastName;
    }

    std::string &User::lastName()
    {
        return _lastName;
    }

    const std::string &User::getEmail() const
    {
        return _email;
    }

    std::string &User::email()
    {
        return _email;
    }

    const std::string &User::getTitle() const
    {
        return _title;
    }

    std::string &User::title()
    {
        return _title;
    }
}