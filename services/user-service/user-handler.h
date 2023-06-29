#ifndef USER_HANDLER_H
#define USER_HANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>
#include <regex>
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/user.h"
#include "../../utils/get-identity.h"
#include "user-config/user-config.h"

class UserHandler : public HTTPRequestHandler
{
private:
    bool checkName(const std::string &name, std::string &reason)
    {
        if (name.length() < 3)
        {
            reason = "Name must be at leas 3 signs";
            return false;
        }

        if (name.find(' ') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        if (name.find('\t') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        return true;
    };

    bool checkEmail(const std::string &email, std::string &reason)
    {
        if (email.find('@') == std::string::npos)
        {
            reason = "Email must contain @";
            return false;
        }

        if (email.find(' ') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        if (email.find('\t') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        return true;
    };

public:
    UserHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        const std::regex getUserByLoginRegex("\\/\\?login=(.*)");
        std::smatch getUserByLoginRegexMatch;
        const std::regex searchUsersRegex("\\/search\\?firstName=(.*)&lastName=(.*)");
        std::smatch searchUsersRegexMatch;

        try
        {
            // не требует аутентификации
            if (request.getURI() == "/" && request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                if (form.has("login") && form.has("password") && form.has("firstName") && form.has("lastName") && form.has("email") && form.has("title"))
                {
                    database::User user;
                    user.login() = form.get("login");
                    user.firstName() = form.get("firstName");
                    user.lastName() = form.get("lastName");
                    user.email() = form.get("email");
                    user.title() = form.get("title");

                    bool checkResult = true;
                    std::string message;
                    std::string reason;

                    if (!checkName(user.getFirstName(), reason))
                    {
                        checkResult = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (!checkName(user.getLastName(), reason))
                    {
                        checkResult = false;
                        message += reason;
                        message += "<br>";
                    }

                    if (!checkEmail(user.getEmail(), reason))
                    {
                        checkResult = false;
                        message += reason;
                        message += "<br>";
                    }

                    std::cout << "checkResult: " << checkResult << std::endl;

                    if (checkResult)
                    {
                        HTMLForm registerForm;
                        registerForm.add("login", form.get("login"));
                        registerForm.add("password", form.get("password"));
                        auto userConfig = UserConfig::get();
                        Poco::Net::HTTPClientSession registerSession(userConfig.getAuthHost(), std::stoul(userConfig.getAuthPort()));
                        Poco::Net::HTTPRequest registerRequest(Poco::Net::HTTPRequest::HTTP_POST, "/register");
                        registerRequest.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
                        registerRequest.set("Accept", "application/json");
                        registerRequest.setKeepAlive(true);
                        std::ostream &registerOstr = registerSession.sendRequest(registerRequest);
                        registerForm.write(registerOstr);
                        Poco::Net::HTTPResponse registerResponse;
                        registerSession.receiveResponse(registerResponse);

                        if (registerResponse.getStatus() == 201)
                        {
                            user.saveToMysql();
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            response.send();
                            return;
                        }

                        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                        root->set("type", "/errors/internal_server_error");
                        root->set("title", "Internal Server Error");
                        root->set("status", 500);
                        root->set("detail", "Failed to create user");
                        root->set("instance", "/user");
                        std::ostream &ostr = response.send();
                        Poco::JSON::Stringifier::stringify(root, ostr);
                        return;
                    }
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/bad_request");
                    root->set("title", "Bad request");
                    root->set("status", 400);
                    root->set("detail", message);
                    root->set("instance", "/user");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/bad_request");
                root->set("title", "Bad request");
                root->set("status", 400);
                root->set("detail", "Provide login, password, firstName, lastName, email, title");
                root->set("instance", "/user");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            // требует аутентификации
            if (!request.has("Authorization"))
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_FORBIDDEN);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/forbidden");
                root->set("title", "Forbidden");
                root->set("status", 403);
                root->set("detail", "Provide basic authorization");
                root->set("instance", "/user");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            auto userConfig = UserConfig::get();
            Poco::Net::HTTPClientSession authSession(userConfig.getAuthHost(), std::stoul(userConfig.getAuthPort()));
            Poco::Net::HTTPRequest authRequest(Poco::Net::HTTPRequest::HTTP_GET, "/check-authorization");
            authRequest.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
            authRequest.set("Accept", "application/json");
            authRequest.setKeepAlive(true);
            authRequest.set("Authorization", request.get("Authorization"));
            authSession.sendRequest(authRequest);
            Poco::Net::HTTPResponse authResponse;
            std::istream &authResponseStream = authSession.receiveResponse(authResponse);
            HTMLForm authForm;
            authForm.read(authResponseStream);
            if (authResponse.getStatus() == 200 && authForm.has("isAuthorized") && authForm.get("isAuthorized") == "true")
            {
                std::cout << "Authorized" << std::endl;
            }
            else
            {
                std::cout << "Unauthorized" << std::endl;
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/unathorized");
                root->set("title", "Unathorized");
                root->set("status", 401);
                root->set("detail", "User unathorized");
                root->set("instance", "/user");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            if (request.getURI() == "/" && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                auto users = database::User::readAll();
                Poco::JSON::Array arr;
                for (auto user : users)
                {
                    arr.add(user.toJSON());
                }

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(arr, ostr);
                return;
            }
            else if (std::regex_match(request.getURI(), searchUsersRegexMatch, searchUsersRegex) && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                const std::string encodedFirstName = searchUsersRegexMatch[1].str(); // если убрать const, то работать не будет
                const std::string encodedLastName = searchUsersRegexMatch[2].str();
                std::string firstName, lastName;
                Poco::URI::decode(encodedFirstName, firstName);
                Poco::URI::decode(encodedLastName, lastName);
                std::cout << "search firstName='" << firstName << "' lastName='" << lastName << "'" << std::endl;
                auto users = database::User::search(firstName, lastName);
                Poco::JSON::Array arr;
                for (auto user : users)
                {
                    arr.add(user.toJSON());
                }
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(arr, ostr);
                return;
            }
            else if (std::regex_match(request.getURI(), getUserByLoginRegexMatch, getUserByLoginRegex) && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                const std::string encodedLogin = getUserByLoginRegexMatch[1].str(); // если убрать const, то работать не будет
                std::string login;
                Poco::URI::decode(encodedLogin, login);
                std::cout << "get by login='" << login << "'" << std::endl;
                auto user = database::User::readByLogin(login);
                if (user.has_value())
                {
                    auto json = user.value().toJSON();

                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(json, ostr);

                    return;
                }

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/not_found");
                root->set("title", "Not Found");
                root->set("status", 404);
                root->set("detail", "No such user");
                root->set("instance", "/user");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/internal_server_error");
        root->set("title", "Bad request");
        root->set("status", 500);
        root->set("detail", "Internal server error");
        root->set("instance", "/user");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
        return;
    }

private:
    std::string _format;
};
#endif