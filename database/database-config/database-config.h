#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H

#include <string>

class DatabaseConfig
{
private:
    DatabaseConfig();
    std::string _dbHost;
    std::string _dbPort;
    std::string _dbLogin;
    std::string _dbPassword;
    std::string _dbDatabase;

public:
    static DatabaseConfig &get();

    std::string &dbPort();
    std::string &dbHost();
    std::string &dbLogin();
    std::string &dbPassword();
    std::string &dbDatabase();

    const std::string &getDbPort() const;
    const std::string &getDbHost() const;
    const std::string &getDbLogin() const;
    const std::string &getDbPassword() const;
    const std::string &getDbDatabase() const;
};

#endif