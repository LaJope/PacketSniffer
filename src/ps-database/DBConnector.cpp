#include <ps-utils/Logger.h>

#include "DBConnector.h"

namespace ps
{
DBConnector::DBConnector()
{
    LOG_INFO("Entering DBConnector constructor ()");
}

DBConnector::DBConnector(const std::string& connectionParameters)
    : m_connectionString(connectionParameters)
{
    LOG_INFO("Entering DBConnector constructor (std::string)");
}

DBConnector::~DBConnector()
{
    disconnect();
}

bool DBConnector::connect()
{
    LOG_INFO("Entering connect");
    try
    {
        m_connection.close();
        m_connection = pqxx::connection(m_connectionString);
        return true;
    }
    catch (const pqxx::broken_connection& exp)
    {
        LOG_ERROR("Could not establish connection with database: '{}'", exp.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Unexpected error during creating database connection: '{}'", e.what());
        return false;
    }
}

void DBConnector::disconnect()
{
    m_connection.close();
}

pqxx::work DBConnector::startTransaction()
{
    return pqxx::work{m_connection};
}

pqxx::result DBConnector::executeQuery(const std::string& query, const pqxx::params& params)
{
    LOG_INFO("Entering execute query");
    try
    {
        pqxx::work txn{m_connection};
        pqxx::result result = txn.exec(query, params);
        txn.commit();
        return result;
    }
    catch (const std::exception& e)
    {
        LOG_THROW("Query execution failed: '{}'", e.what());
    }

    return {};
}
} // namespace ps
