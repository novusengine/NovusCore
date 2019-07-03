#pragma once
#include <NovusTypes.h>
#include <shared_mutex>
#include <assert.h>

class BaseDatabaseCache
{
public:
    BaseDatabaseCache(){};
    ~BaseDatabaseCache(){};

    virtual void Load() = 0;
    virtual void LoadAsync() = 0;

    virtual void Save() = 0;
    virtual void SaveAsync() = 0;

private:
protected:
    std::shared_mutex _accessMutex;
};