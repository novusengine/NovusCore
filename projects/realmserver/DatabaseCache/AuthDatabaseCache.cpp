#include "AuthDatabaseCache.h"
#include <Database/DatabaseConnector.h>
#include <Database/PreparedStatement.h>

AuthDatabaseCache::AuthDatabaseCache()
{
}

AuthDatabaseCache::~AuthDatabaseCache()
{
}

void AuthDatabaseCache::Load()
{
    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, connector);
    assert(result);
    if (!result)
        return;

    amy::result_set resultSet;
    connector->Query("SELECT accountGuid, type, timestamp, data FROM account_data", resultSet);

    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            AccountData newAccountData(this);
            newAccountData.accountGuid = row[0].as<amy::sql_int_unsigned>();
            newAccountData.type = row[1].as<amy::sql_int_unsigned>();
            newAccountData.timestamp = row[2].as<amy::sql_int_unsigned>();
            newAccountData.data = row[3].as<amy::sql_blob>();
            newAccountData.loaded = true;

            _accessMutex.lock();
            _accountDataCache.insert({ newAccountData.accountGuid, newAccountData });
            _accessMutex.unlock();
        }
    }
}

void AuthDatabaseCache::LoadAsync()
{
}

void AuthDatabaseCache::Save()
{
}

void AuthDatabaseCache::SaveAsync()
{

}

void AuthDatabaseCache::SaveAccountData(u32 accountGuid, u32 type)
{
    DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [this, accountGuid, type](std::shared_ptr<DatabaseConnector> & connector)
    {
        AccountData accountData = _accountDataCache[accountGuid][type];
        if (accountData.loaded)
        {
            PreparedStatement accountDataStmt("INSERT INTO account_data(accountGuid, type, timestamp, data) VALUES({u}, {u}, {u}, {s}) ON DUPLICATE KEY UPDATE timestamp={u}, data={s};");
            accountDataStmt.Bind(accountData.accountGuid);
            accountDataStmt.Bind(accountData.type);
            accountDataStmt.Bind(accountData.timestamp);
            accountDataStmt.Bind(accountData.data);
            accountDataStmt.Bind(accountData.timestamp);
            accountDataStmt.Bind(accountData.data);

            connector->Execute(accountDataStmt);
        }
    });
}

bool AuthDatabaseCache::GetAccountData(u32 accountGuid, u32 type, AccountData& output)
{
    auto cache = _accountDataCache.find(accountGuid);
    if (cache != _accountDataCache.end())
    {
        AccountData accountData = cache->second[type];
        if (accountData.loaded)
        {
            output = accountData;
            return true;
        }
    }

    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, connector);
    assert(result);
    if (!result)
        return false;

    PreparedStatement stmt("SELECT * FROM account_data WHERE accountGuid = {u} AND type = {u};");
    stmt.Bind(accountGuid);
    stmt.Bind(type);

    amy::result_set resultSet;
    connector->Query(stmt, resultSet);

    AccountData newAccountData(this);
    if (resultSet.affected_rows() == 0)
    {
        newAccountData.accountGuid = accountGuid;
        newAccountData.type = type;
        newAccountData.timestamp = 0;
        newAccountData.data = "";
    }
    else
    {
        newAccountData.accountGuid = resultSet[0][0].as<amy::sql_int_unsigned>();
        newAccountData.type = resultSet[0][1].as<amy::sql_int_unsigned>();
        newAccountData.timestamp = resultSet[0][2].as<amy::sql_int_unsigned>();
        newAccountData.data = resultSet[0][3].as<amy::sql_blob>();
    }
    newAccountData.loaded = true;

    _accessMutex.lock();
    _accountDataCache.insert({ accountGuid, newAccountData });
    _accessMutex.unlock();

    output = newAccountData;
    return true;
}

void AccountData::UpdateCache()
{
    _cache->_accountDataCache[accountGuid][type] = *this;
    _cache->SaveAccountData(accountGuid, type);
}
