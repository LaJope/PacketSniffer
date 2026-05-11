#pragma once

#include <memory>
#include <string>

#include <pqxx/pqxx>

namespace ps
{
class DBConnector
{
  public:
    using Ptr = std::shared_ptr<DBConnector>;

    DBConnector();
    DBConnector(const std::string& connectionParameters);
    ~DBConnector();

    bool connect();
    void disconnect();

    pqxx::work startTransaction();

    pqxx::result executeQuery(const std::string& query, const pqxx::params& params = pqxx::params{});

  private:
    pqxx::connection m_connection;
    std::string m_connectionString = "user=postgres password=postgres host=localhost port=5432 dbname=postgres";
};
} // namespace ps
