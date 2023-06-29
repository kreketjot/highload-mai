#ifndef POST_H
#define POST_H

#include <string>
#include <vector>
#include <optional>
#include <Poco/JSON/Object.h>

namespace database
{
    class Post
    {
    private:
        long _id;
        std::string _title;
        std::string _content;
        long _user_id;

    public:
        static Post fromJSON(const std::string &str);

        long id() const;
        const std::string &title() const;
        const std::string &content() const;
        long user_id() const;

        long &id();
        std::string &title();
        std::string &content();
        long &user_id();

        static void init();
        static std::optional<Post> read_by_id(long id);
        static std::vector<Post> read_all();
        static std::vector<Post> read_by_author(long userId);
        void save_to_mysql();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif