#pragma once
#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <vector>

struct OpcodePacket
{
    u16 opcode;
    bool handled;
    Common::ByteBuffer data;
};

struct ConnectionComponent 
{
	u32 accountGuid;
	u64 characterGuid;
    bool isInitialized;
    std::vector<OpcodePacket> packets;
};