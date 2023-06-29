#ifndef MESSANGER_HANDLER_H
#define MESSANGER_HANDLER_H

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

#include "../../database/chat-message.h"
#include "../../utils/get-identity.h"
#include "messanger-config/messanger-config.h"

class MessangerHandler : public HTTPRequestHandler
{
public:
    MessangerHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        const std::regex chatByLoginRegex("\\/\\?login=(.*)");
        std::smatch chatByLoginRegexMatch;

        try
        {
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
                root->set("instance", "/messanger");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            auto messangerConfig = MessangerConfig::get();
            Poco::Net::HTTPClientSession authSession(messangerConfig.getAuthHost(), std::stoul(messangerConfig.getAuthPort()));
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
                root->set("instance", "/messanger");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            std::string tmp, identity, login1, password;
            request.getCredentials(tmp, identity);
            getIdentity(identity, login1, password);
            std::cout << "login1='" << login1 << "'" << std::endl;

            if (std::regex_match(request.getURI(), chatByLoginRegexMatch, chatByLoginRegex))
            {
                const std::string encodedLogin = chatByLoginRegexMatch[1].str(); // если убрать const, то работать не будет
                std::string login2;
                Poco::URI::decode(encodedLogin, login2);
                std::cout << "login2='" << login2 << "'" << std::endl;

                if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
                {
                    auto chatMessages = database::ChatMessage::readAllByChat(login1, login2);
                    Poco::JSON::Array arr;
                    for (auto message : chatMessages)
                    {
                        arr.add(message.toJSON());
                    }

                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(arr, ostr);
                    return;
                }
                else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST && form.has("content"))
                {
                    database::ChatMessage message;
                    message.sender() = login1;
                    message.recipient() = login2;
                    message.content() = form.get("content");
                    message.saveToMysql();

                    response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
                    response.setChunkedTransferEncoding(true);
                    response.send();
                    return;
                }
            }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/bad_request");
        root->set("title", "Bad request");
        root->set("status", 400);
        root->set("detail", "Provide login");
        root->set("instance", "/messanger");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
        return;
    }

private:
    std::string _format;
};
#endif