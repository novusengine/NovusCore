#pragma once
#include "BaseDatabaseCache.h"
#include <vector>
#include <robin_hood.h>

class AuthDatabaseCache;
// account_data table in DB
struct AccountData
{
    AccountData() { }
    AccountData(AuthDatabaseCache* cache) { _cache = cache; }
    AccountData(const AccountData& inData)
    {
        accountGuid = inData.accountGuid;
        type = inData.type;
        timestamp = inData.timestamp;
        data = inData.data;
        loaded = inData.loaded;
        _cache = inData._cache;
    }

    u32 accountGuid;
    u32 type;
    u32 timestamp;
    std::string data;

    bool loaded = false;
    void UpdateCache();
private:
    AuthDatabaseCache* _cache;
};

class AuthDatabaseCache : BaseDatabaseCache
{
public:
    AuthDatabaseCache();
    ~AuthDatabaseCache();

    void Load() override;
    void LoadAsync() override;
    void Save() override;
    void SaveAsync() override;

    // Save Account Data
    void SaveAccountData(u32 accountGuid, u32 type);

    // Account Data cache
    bool GetAccountData(u32 accountGuid, u32 type, AccountData& output);

private:
    friend AccountData;

    robin_hood::unordered_map<u32, AccountData[8]> _accountDataCache;
};