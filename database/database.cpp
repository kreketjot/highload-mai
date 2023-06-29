#include "database.h"
#include "database-config/database-config.h"

namespace database
{
    Database::Database()
    {
        auto config = DatabaseConfig::get();
        _connection_string += "host=";
        _connection_string += config.getDbHost();
        _connection_string += ";user=";
        _connection_string += config.getDbLogin();
        _connection_string += ";db=";
        _connection_string += config.getDbDatabase();
        _connection_string += ";port=";
        _connection_string += config.getDbPort();
        _connection_string += ";password=";
        _connection_string += config.getDbPassword();

        std::cout << "Connection string:" << _connection_string << std::endl;
        Poco::Data::MySQL::Connector::registerConnector();
        _pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::MySQL::Connector::KEY, _connection_string);
    }

    Database &Database::get()
    {
        static Database _instance;
        return _instance;
    }

    Poco::Data::Session Database::createSession()
    {
        return Poco::Data::Session(_pool->get());
    }

    size_t Database::getMaxShard()
    {
        return 2;
    }

    std::vector<std::string> Database::getAllHints()
    {
        std::vector<std::string> result;
        for (size_t i = 0; i != getMaxShard(); ++i)
        {
            std::string shard_name = "-- sharding:";
            shard_name += std::to_string(i);
            result.push_back(shard_name);
        }
        return result;
    }

    std::string Database::shardingHint(const std::string &user)
    {
        size_t shard_number = std::hash<std::string>{}(user) % getMaxShard();

        std::string result = "-- sharding:";
        result += std::to_string(shard_number);
        return result;
    }
}