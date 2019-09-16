#pragma once
#include <NovusTypes.h>
#include <cassert>

#include "NetPacket.h"

#define OpcodeCount 512
typedef bool (*MessageHandlerFunc)(NetPacket*);
class MessageHandler
{
public:
    MessageHandler();

    static void Create()
    {
        assert(_instance == nullptr);
        _instance = new MessageHandler();
    }
    static MessageHandler* Instance()
    {
        assert(_instance != nullptr);
        return _instance;
    }

    void SetMessageHandler(u32 opcode, MessageHandlerFunc func);
    bool CallHandler(NetPacket* packet);

private:
    static MessageHandler* _instance;
    MessageHandlerFunc handlers[OpcodeCount];
};