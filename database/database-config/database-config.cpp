#include "database-config.h"

DatabaseConfig::DatabaseConfig()
{
    auto getenv = [&](const char *p)
    { return std::getenv(p) ? std::getenv(p) : ""; };

    _dbHost = getenv("DB_HOST");
    _dbPort = getenv("DB_PORT");
    _dbLogin = getenv("DB_LOGIN");
    _dbPassword = getenv("DB_PASSWORD");
    _dbDatabase = getenv("DB_DATABASE");
}

DatabaseConfig &DatabaseConfig::get()
{
    static DatabaseConfig _instance;
    return _instance;
}

const std::string &DatabaseConfig::getDbPort() const
{
    return _dbPort;
}

const std::string &DatabaseConfig::getDbHost() const
{
    return _dbHost;
}

const std::string &DatabaseConfig::getDbLogin() const
{
    return _dbLogin;
}

const std::string &DatabaseConfig::getDbPassword() const
{
    return _dbPassword;
}
const std::string &DatabaseConfig::getDbDatabase() const
{
    return _dbDatabase;
}

std::string &DatabaseConfig::dbPort()
{
    return _dbPort;
}

std::string &DatabaseConfig::dbHost()
{
    return _dbHost;
}

std::string &DatabaseConfig::dbLogin()
{
    return _dbLogin;
}

std::string &DatabaseConfig::dbPassword()
{
    return _dbPassword;
}

std::string &DatabaseConfig::dbDatabase()
{
    return _dbDatabase;
}
