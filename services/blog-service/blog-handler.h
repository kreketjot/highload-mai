#ifndef BLOG_HANDLER_H
#define BLOG_HANDLER_H

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

#include "../../database/blog-post.h"
#include "../../utils/get-identity.h"
#include "blog-config/blog-config.h"

class BlogHandler : public HTTPRequestHandler
{
public:
    BlogHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        const std::regex getBlogByLoginRegex("\\/\\?login=(.*)");
        std::smatch getBlogByLoginRegexMatch;

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
                root->set("instance", "/blog");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            auto blogConfig = BlogConfig::get();
            Poco::Net::HTTPClientSession authSession(blogConfig.getAuthHost(), std::stoul(blogConfig.getAuthPort()));
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
                root->set("instance", "/blog");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }

            if (std::regex_match(request.getURI(), getBlogByLoginRegexMatch, getBlogByLoginRegex) && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                const std::string encodedLogin = getBlogByLoginRegexMatch[1].str(); // если убрать const, то работать не будет
                std::string login;
                Poco::URI::decode(encodedLogin, login);
                std::cout << "read blog login='" << login << "'" << std::endl;
                auto blogPosts = database::BlogPost::readAllByAuthor(login);
                Poco::JSON::Array arr;
                for (auto blogPost : blogPosts)
                {
                    arr.add(blogPost.toJSON());
                }
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(arr, ostr);
                return;
            }
            else if (request.getURI() == "/" && request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST && form.has("title") && form.has("content"))
            {
                std::string tmp, identity, login, password;
                request.getCredentials(tmp, identity);
                getIdentity(identity, login, password);
                database::BlogPost blogPost;
                blogPost.author() = login;
                blogPost.title() = form.get("title");
                blogPost.content() = form.get("content");
                blogPost.saveToMysql();

                response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
                response.setChunkedTransferEncoding(true);
                response.send();
                return;
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
        root->set("instance", "/blog");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
        return;
    }

private:
    std::string _format;
};
#endif