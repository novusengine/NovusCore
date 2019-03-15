#pragma once
#include <NovusTypes.h>
#include "../Message.h"
#include "Utils/ConcurrentQueue.h"

struct CreatePlayerQueueSingleton
{
    ConcurrentQueue<Message>* newEntityQueue;
};