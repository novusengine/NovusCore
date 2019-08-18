#include "AccountPacketUtils.h"
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../WorldNodeHandler.h"
#include "../../ECS/Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../../ECS/Components/PlayerConnectionComponent.h"

#include "../../Utils/ServiceLocator.h"
#include <tracy/Tracy.hpp>
#include <zlib.h>

void AccountPacketUtils::RegisterAccountPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES, HandleReadyForAccountDataTimes);
    messageHandler->SetMessageHandler(Opcode::CMSG_REQUEST_ACCOUNT_DATA, HandleRequestAccountData);
    messageHandler->SetMessageHandler(Opcode::CMSG_UPDATE_ACCOUNT_DATA, HandleUpdateAccountData);
}

bool AccountPacketUtils::HandleReadyForAccountDataTimes(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    /* Packet Structure */
    // UInt32:  Server Time (time(nullptr))
    // UInt8:   Unknown Byte Value
    // UInt32:  Mask for the account data fields
    
    CharacterDatabaseCacheSingleton& characterDatabase = ServiceLocator::GetMainRegistry()->ctx<CharacterDatabaseCacheSingleton>();
    std::shared_ptr<ByteBuffer> accountDataTimes = ByteBuffer::Borrow<41>();

    u32 mask = 0x15;
    accountDataTimes->PutU32(static_cast<u32>(time(nullptr)));
    accountDataTimes->PutU8(1); // bitmask blocks count
    accountDataTimes->PutU32(mask);

    for (u32 i = 0; i < 8; ++i)
    {
        if (mask & (1 << i))
        {
            CharacterData characterData;
            if (characterDatabase.cache->GetCharacterData(playerConnectionComponent->characterGuid, i, characterData))
            {
                accountDataTimes->PutU32(characterData.timestamp);
            }
        }
    }

    playerConnectionComponent->socket->SendPacket(accountDataTimes.get(), Opcode::SMSG_ACCOUNT_DATA_TIMES);
    return true;
}
bool AccountPacketUtils::HandleRequestAccountData(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    return true;
}
bool AccountPacketUtils::HandleUpdateAccountData(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    u32 type, timestamp, decompressedSize;
    packet->data->GetU32(type);
    packet->data->GetU32(timestamp);
    packet->data->GetU32(decompressedSize);

    if (type > 8)
        return true;

    CharacterDatabaseCacheSingleton& characterDatabase = ServiceLocator::GetMainRegistry()->ctx<CharacterDatabaseCacheSingleton>();
    bool characterDataUpdate = ((1 << type) & CHARACTER_DATA_CACHE_MASK);

    // Clear Data
    if (decompressedSize == 0)
    {
        if (characterDataUpdate)
        {
            CharacterData characterData;
            if (characterDatabase.cache->GetCharacterData(playerConnectionComponent->characterGuid, type, characterData))
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
            return true;

        std::shared_ptr<ByteBuffer> DataInfo = ByteBuffer::Borrow<1024>();
        DataInfo->Size = packet->data->Size - packet->data->ReadData;
        DataInfo->PutBytes(packet->data->GetInternalData() + packet->data->ReadData, DataInfo->Size);

        uLongf uSize = decompressedSize;
        u32 pos = static_cast<u32>(DataInfo->ReadData);

        std::shared_ptr<ByteBuffer> dataInfo = ByteBuffer::Borrow<8192>();
        if (uncompress(dataInfo->GetInternalData(), &uSize, DataInfo->GetInternalData() + pos, static_cast<uLong>(DataInfo->Size - pos)) != Z_OK)
            return true;

        dataInfo->WrittenData = static_cast<size_t>(decompressedSize);

        std::string finalData = "";
        dataInfo->GetString(finalData, static_cast<i32>(decompressedSize));

        if (characterDataUpdate)
        {
            CharacterData characterData;
            if (characterDatabase.cache->GetCharacterData(playerConnectionComponent->characterGuid, type, characterData))
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

    playerConnectionComponent->socket->SendPacket(updateAccountDataComplete.get(), Opcode::SMSG_UPDATE_ACCOUNT_DATA_COMPLETE);
    return true;
}