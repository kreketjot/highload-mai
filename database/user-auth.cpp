#include "user-auth.h"
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
    void UserAuth::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `UserAuth` ("
                        << "`login` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL UNIQUE, "
                        << "`password` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL"
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
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr UserAuth::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    UserAuth UserAuth::fromJSON(const std::string &str)
    {
        UserAuth userAuth;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        userAuth.login() = object->getValue<std::string>("login");
        userAuth.password() = object->getValue<std::string>("password");

        return userAuth;
    }

    bool UserAuth::auth(std::string login, std::string password)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement select(session);
            std::string selectedLogin;
            select << "SELECT login FROM UserAuth "
                   << "WHERE login=? AND password=?;",
                into(selectedLogin),
                use(login),
                use(password),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return true;
            return false;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection: " << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
        }
        return false;
    }

    void UserAuth::saveToMysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO UserAuth (login, password) VALUES(?, ?);",
                use(_login),
                use(_password);

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

    void UserAuth::deleteUser(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement deleteStatement(session);
            deleteStatement << "DELETE FROM UserAuth "
                            << "WHERE login=?;",
                use(login),
                range(0, 1); //  iterate over result set one row at a time

            deleteStatement.execute();
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection: " << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement: " << e.what() << " " << e.message() << std::endl;
        }
    }

    const std::string &UserAuth::getLogin() const
    {
        return _login;
    }

    std::string &UserAuth::login()
    {
        return _login;
    }

    const std::string &UserAuth::getPassword() const
    {
        return _password;
    }

    std::string &UserAuth::password()
    {
        return _password;
    }
}