#pragma once
#include "NovusTypes.h"

class NovusTypeHeader
{
public:
    NovusTypeHeader()
        : typeID(-1)
        , typeVersion(-1)
    {

    }

    NovusTypeHeader(u32 id, u32 version)
    {
        typeID = id;
        typeVersion = version;
    }
    
    u32 typeID;
    u32 typeVersion;

    bool operator==(const NovusTypeHeader& other)
    {
        return typeID == other.typeID && typeVersion == other.typeVersion;
    }

    bool operator!=(const NovusTypeHeader& other)
    {
        return typeID != other.typeID || typeVersion != other.typeVersion;
    }

};