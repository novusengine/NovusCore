#pragma once
#include <NovusTypes.h>
#include "../Message.h"
#include "Utils/ConcurrentQueue.h"

struct ExpiredPlayerData
{
    u32 account;
    u64 guid;
};

struct DeletePlayerQueueSingleton
{
    ConcurrentQueue<ExpiredPlayerData>* expiredEntityQueue;
};