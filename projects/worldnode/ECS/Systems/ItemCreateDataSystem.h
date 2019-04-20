/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>

#include "../NovusEnums.h"
#include "../Utils/UpdateData.h"
#include "../Utils/UpdateMask.h"

#include "../Components/ItemDataComponent.h"
#include "../Components/ItemInitializeComponent.h"
#include "../Components/ItemFieldDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

namespace ItemCreateDataSystem
{
    Common::ByteBuffer BuildItemCreateData(u64 itemGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, ItemFieldDataComponent& itemFieldData, u16& opcode)
    {
        Common::ByteBuffer buffer(500);
        buffer.Write<u8>(updateType);
        buffer.AppendGuid(itemGuid);
        buffer.Write<u8>(1); // TYPEID_ITEM

        // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
        buffer.Write<u16>(updateFlags);

        if (updateFlags & UPDATEFLAG_LOWGUID)
        {
            buffer.Write<u32>(1);
        }

        Common::ByteBuffer fieldBuffer;
        fieldBuffer.Resize(256);
        UpdateMask<64> updateMask(ITEM_END);

        u32* flags = ItemUpdateFieldFlags;
        u16 fieldNotifyFlags = UF_FLAG_DYNAMIC;

        for (u16 index = 0; index < ITEM_END; index++)
        {
            if (fieldNotifyFlags & flags[index] ||
                ((updateType == UPDATETYPE_VALUES ? itemFieldData.changesMask.IsSet(index) : itemFieldData.itemFields.ReadAt<i32>(index * 4)) &&
                (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);
                fieldBuffer.Write(itemFieldData.itemFields.GetDataPointer() + index * 4, 4);
            }
        }

        buffer.Write<u8>(updateMask.GetBlocks());
        updateMask.AddTo(buffer);
        buffer.Append(fieldBuffer);

        UpdateData updateData;
        updateData.AddBlock(buffer);

        Common::ByteBuffer tempBuffer;
        updateData.Build(tempBuffer, opcode);

        return tempBuffer;
    }

    void Update(entt::registry &registry)
    {
        SingletonComponent& singleton = registry.ctx<SingletonComponent>();

        auto itemView = registry.view<ItemInitializeComponent, ItemFieldDataComponent>();
        if (!itemView.empty())
        {
            itemView.each([&](const auto, ItemInitializeComponent& itemInitializeData, ItemFieldDataComponent& itemFieldData)
            {
                /* Build Self Packet, must be sent immediately */
                u8 updateType = UPDATETYPE_CREATE_OBJECT;
                u16 selfUpdateFlag = UPDATEFLAG_LOWGUID;
                u32 selfVisibleFlags = (UF_FLAG_PUBLIC | UF_FLAG_OWNER);
                u16 buildOpcode = 0;

                Common::ByteBuffer selfItemUpdate = BuildItemCreateData(itemInitializeData.itemGuid, updateType, selfUpdateFlag, selfVisibleFlags, itemFieldData, buildOpcode);
                //novusConnection.SendPacket(itemInitializeData.accountGuid, selfItemUpdate, buildOpcode);

                Common::ByteBuffer itemPushResult;
                itemPushResult.Write<u64>(itemInitializeData.characterGuid);
                itemPushResult.Write<u32>(0); // Received:     0 = Looted,   1 = By NPC
                itemPushResult.Write<u32>(0); // Created:      0 = Received, 1 = Created
                itemPushResult.Write<u32>(1); // Show in chat: 0 = No Print, 1 = Print
                itemPushResult.Write<u8>(itemInitializeData.bagSlot); // BagSlot
                itemPushResult.Write<u32>(itemInitializeData.bagPosition); // BagPosition
                itemPushResult.Write<u32>(itemInitializeData.itemGuid.GetEntry());
                itemPushResult.Write<u32>(0);
                itemPushResult.Write<u32>(0);
                itemPushResult.Write<u32>(1);
                //novusConnection.SendPacket(itemInitializeData.accountGuid, itemPushResult, Common::Opcode::SMSG_ITEM_PUSH_RESULT);
            });
            // Remove ItemInitializeComponent from all entities (They've just been handled above)
            registry.reset<ItemInitializeComponent>();
        }
    }
}