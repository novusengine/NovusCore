#include <NovusTypes.h>
#include <Utils\ByteBuffer.h>

struct NetPacket
{
    u32 opcode = 0;
    bool handled = false;
    std::shared_ptr<ByteBuffer> data = nullptr;
};