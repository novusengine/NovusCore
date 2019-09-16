#pragma once
#include <NovusTypes.h>
#include <cassert>

#include "NetPacket.h"

typedef bool (*MessageHandlerFunc)(NetPacket*);

template <size_t OpcodeCount>
class _MessageHandler
{
public:
    _MessageHandler();

    static void Create(size_t opcodeNums)
    {
        assert(_instance == nullptr);
        _instance = new _MessageHandler<opcodeNums>();
    }
    static _MessageHandler* Instance()
    {
        assert(_instance != nullptr);
        return _instance;
    }

    void SetMessageHandler(u32 opcode, MessageHandlerFunc func);
    bool CallHandler(NetPacket* packet);

private:
    static _MessageHandler* _instance;
    const size_t opcodeCount = OpcodeCount;
    MessageHandlerFunc handlers[OpcodeCount];
};

using MessageHandler = _MessageHandler<512>;