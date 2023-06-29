#ifndef AUTH_HANDLER_H
#define AUTH_HANDLER_H

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

#include "../../database/user-auth.h"
#include "../../utils/get-identity.h"

class AuthHandler : public HTTPRequestHandler
{
public:
    AuthHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            if (request.getURI() == "/check-authorization" && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                std::string scheme;
                std::string info;
                request.getCredentials(scheme, info);
                std::cout << "scheme: " << scheme << " identity: " << info << std::endl;

                std::string login, password;
                if (scheme == "Basic")
                {
                    getIdentity(info, login, password);
                    bool isAuthorized = database::UserAuth::auth(login, password);
                    std::cout << "isAuthorized: " << isAuthorized << std::endl;
                    Poco::Net::HTMLForm authForm;
                    authForm.add("isAuthorized", isAuthorized ? "true" : "false");
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    std::ostream &ostr = response.send();
                    authForm.write(ostr);
                    return;
                }

                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/bad_request");
                root->set("title", "Bad request");
                root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                root->set("detail", "Allowed Basic authorization only");
                root->set("instance", "/auth");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }
            else if (request.getURI() == "/register" && request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                if (form.has("login") && form.has("password"))
                {
                    std::cout << "login: " << form.get("login") << " password: " << form.get("password") << std::endl;
                    database::UserAuth userAuth;
                    userAuth.login() = form.get("login");
                    userAuth.password() = form.get("password");
                    userAuth.saveToMysql();

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
        root->set("detail", "Bad request");
        root->set("instance", "/auth");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
        return;
    }

private:
    std::string _format;
};
#endif