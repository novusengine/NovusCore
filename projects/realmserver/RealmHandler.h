#pragma once
#include <NovusTypes.h>

class RealmHandler
{
public:
    static RealmHandler* Instance()
    {
        if (!_instance)
        {
            _instance = new RealmHandler();
        }

        return _instance;
    }

    i32 worldNodeAddress;
    i16 worldNodePort;

private:
    RealmHandler() {}
    ~RealmHandler() {}

    static RealmHandler* _instance;
};
