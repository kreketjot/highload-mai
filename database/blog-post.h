#ifndef BLOG_POST_H
#define BLOG_POST_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class BlogPost
    {
    private:
        long _id;
        std::string _author;
        std::string _title;
        std::string _content;

    public:
        static BlogPost fromJSON(const std::string &str);

        long getId() const;
        const std::string &getAuthor() const;
        const std::string &getTitle() const;
        const std::string &getContent() const;

        long &id();
        std::string &author();
        std::string &title();
        std::string &content();

        static void init();
        static std::vector<BlogPost> readAllByAuthor(std::string user);
        void saveToMysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif