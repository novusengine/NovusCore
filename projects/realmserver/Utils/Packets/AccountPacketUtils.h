#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <Database\DatabaseConnector.h>
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../RealmHandler.h"
#include "../../Connections/RealmConnection.h"
#include <zlib.h>

namespace AccountPacketUtils
{
bool HandleReadyForAccountDataTimes(RealmConnection* connection)
{
    /* Packet Structure */
    // UInt32:  Server Time (time(nullptr))
    // UInt8:   Unknown Byte Value
    // UInt32:  Mask for the account data fields

    std::shared_ptr<ByteBuffer> accountDataTimes = ByteBuffer::Borrow<41>();

    u32 mask = 0x15;
    accountDataTimes->PutU32(static_cast<u32>(time(nullptr)));
    accountDataTimes->PutU8(1); // bitmask blocks count
    accountDataTimes->PutU32(mask);

    AccountData accountData;
    for (u32 i = 0; i < 8; ++i)
    {
        if (mask & (1 << i))
        {
            if (connection->authCache.GetAccountData(connection->account, i, accountData))
            {
                accountDataTimes->PutU32(accountData.timestamp);
            }
            else
            {
                accountDataTimes->PutU32(0);
            }
        }
    }

    connection->SendPacket(accountDataTimes.get(), Opcode::SMSG_ACCOUNT_DATA_TIMES);
    return true;
}
bool HandleRequestAccountData(RealmConnection* connection)
{
    return true;
}
bool HandleUpdateAccountData(RealmConnection* connection)
{
    ByteBuffer& buffer = connection->GetPacketBuffer();
    u32 type, timestamp, decompressedSize;
    buffer.GetU32(type);
    buffer.GetU32(timestamp);
    buffer.GetU32(decompressedSize);

    if (type > 8)
        return false;

    bool accountDataUpdate = ((1 << type) & 0x15);

    // Clear Data
    if (decompressedSize == 0)
    {
        if (accountDataUpdate)
        {
            AccountData accountData;
            if (connection->authCache.GetAccountData(connection->account, type, accountData))
            {
                accountData.timestamp = 0;
                accountData.data = "";
                accountData.UpdateCache();
            }
        }
        else
        {
            CharacterData characterData;
            if (connection->charCache.GetCharacterData(connection->characterGuid, type, characterData))
            {
                characterData.timestamp = 0;
                characterData.data = "";
                characterData.UpdateCache();
            }
        }
    }
    else
    {
        if (decompressedSize > 0xFFFF)
            return false;

        ByteBuffer DataInfo(nullptr, buffer.Size - buffer.ReadData);
        DataInfo.PutBytes(buffer.GetInternalData() + buffer.ReadData, DataInfo.Size);

        uLongf uSize = decompressedSize;
        u32 pos = static_cast<u32>(DataInfo.ReadData);

        ByteBuffer dataInfo(nullptr, decompressedSize);
        if (uncompress(dataInfo.GetInternalData(), &uSize, DataInfo.GetInternalData() + pos, static_cast<uLong>(DataInfo.Size - pos)) != Z_OK)
            return false;

        dataInfo.WrittenData = static_cast<size_t>(decompressedSize);

        std::string finalData = "";
        dataInfo.GetString(finalData, static_cast<i32>(decompressedSize));

        if (accountDataUpdate)
        {
            AccountData accountData;
            if (connection->authCache.GetAccountData(connection->account, type, accountData))
            {
                accountData.timestamp = timestamp;
                accountData.data = finalData;
                accountData.UpdateCache();
            }
        }
        else
        {
            CharacterData characterData;
            if (connection->charCache.GetCharacterData(connection->characterGuid, type, characterData))
            {
                characterData.timestamp = timestamp;
                characterData.data = finalData;
                characterData.UpdateCache();
            }
        }
    }

    std::shared_ptr<ByteBuffer> updateAccountDataComplete = ByteBuffer::Borrow<8>();
    updateAccountDataComplete->PutU32(type);
    updateAccountDataComplete->PutU32(0);

    connection->SendPacket(updateAccountDataComplete.get(), Opcode::SMSG_UPDATE_ACCOUNT_DATA_COMPLETE);
    return true;
}

void RegisterAccountPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES, HandleReadyForAccountDataTimes);
    messageHandler->SetMessageHandler(Opcode::CMSG_UPDATE_ACCOUNT_DATA, HandleRequestAccountData);
    messageHandler->SetMessageHandler(Opcode::CMSG_REQUEST_ACCOUNT_DATA, HandleUpdateAccountData);
}
} // namespace SocketPacketUtils