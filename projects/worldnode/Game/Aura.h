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
#include <Networking/ByteBuffer.h>
#include <Database/Cache/DBCDatabaseCache.h>

#include "../NovusEnums.h"

#include "../Scripting/AuraFunctions.h"

enum AuraServerFlags
{
    AURA_SERVER_FLAG_NONE = 0x00,
    AURA_SERVER_FLAG_IS_APPLIED = 0x01,
    AURA_SERVER_FLAG_IS_SERVERSIDE = 0x02,
    AURA_SERVER_FLAG_NEED_UPDATE = 0x04
};
struct Aura
{
    Aura() : entityId(0), unitGuid(0), spellData(), effectIndex(0), serverFlags(AURA_SERVER_FLAG_NONE), slot(0), clientFlags(AURA_CLIENT_FLAG_NONE), casterLevel(0), casterGuid(0), maxDuration(0), duration(0) {}

    // Information for the server
    u32 entityId;
    u64 unitGuid;
    SpellData spellData;
    u8 effectIndex;
    u8 serverFlags;

    // Information for the client
    u8 slot;
    u8 clientFlags;
    u8 casterLevel;
    u64 casterGuid;
    i32 maxDuration;
    i32 duration;

    bool IsApplied()
    {
        return (serverFlags & AURA_SERVER_FLAG_IS_APPLIED);
    }
    bool IsServerside()
    {
        return (serverFlags & AURA_SERVER_FLAG_IS_SERVERSIDE);
    }
    bool NeedUpdate()
    {
        return (serverFlags & AURA_SERVER_FLAG_NEED_UPDATE);
    }
    void SetApplied(bool state)
    {
        if (state)
        {
            serverFlags |= AURA_SERVER_FLAG_IS_APPLIED;

            assert(entityId != 0);
            AngelScriptPlayer asPlayer(entityId);
            AngelScriptAura asAura(this);
            AuraHooks::CallHook(AuraHooks::Hooks::HOOK_ON_AURA_APPLIED, &asPlayer, &asAura);
        }
        else
        {
            serverFlags &= ~AURA_SERVER_FLAG_IS_APPLIED;

            if (spellData.EffectApplyAuraName[effectIndex] == SPELL_AURA_FLY)
            {
            }
        }
    }
    void SetUpdate(bool state)
    {
        assert(!IsServerside());

        if (state)
        {
            serverFlags |= AURA_SERVER_FLAG_NEED_UPDATE;
        }
        else
        {
            serverFlags &= ~AURA_SERVER_FLAG_NEED_UPDATE;
        }
    }

    bool IsEffect_1()
    {
        return (clientFlags & AURA_CLIENT_FLAG_EFFECT_INDEX_1);
    }
    bool IsEffect_2()
    {
        return (clientFlags & AURA_CLIENT_FLAG_EFFECT_INDEX_2);
    }
    bool IsEffect_3()
    {
        return (clientFlags & AURA_CLIENT_FLAG_EFFECT_INDEX_3);
    }
    bool IsSelfCast()
    {
        return (clientFlags & AURA_CLIENT_FLAG_IS_CASTER);
    }
    bool IsPositive()
    {
        return (clientFlags & AURA_CLIENT_FLAG_IS_POSITIVE);
    }
    bool HasDuration()
    {
        return (clientFlags & AURA_CLIENT_FLAG_HAS_DURATION);
    }
    bool IsNegative()
    {
        return (clientFlags & AURA_CLIENT_FLAG_IS_NEGATIVE);
    }

    void UnApply()
    {
        assert(IsApplied());

        SetApplied(false);

        if (!IsServerside())
            SetUpdate(true);
    }
    void CreateAuraUpdate(std::shared_ptr<ByteBuffer> auraBuffer, bool includeGuid = false)
    {
        assert(NeedUpdate());

        if (includeGuid)
        {
            auraBuffer->PutGuid(unitGuid);
        }

        auraBuffer->PutU8(slot);
        if (IsApplied())
        {
            auraBuffer->PutU32(spellData.Id);
            auraBuffer->PutU8(clientFlags);
            auraBuffer->PutU8(casterLevel);
            auraBuffer->PutU8(spellData.StackAmount);

            if (!IsSelfCast())
            {
                auraBuffer->PutGuid(casterGuid);
            }

            if (HasDuration())
            {
                auraBuffer->PutI32(maxDuration);
                auraBuffer->PutI32(duration);
            }
        }
        else
        {
            auraBuffer->PutU32(0);
        }

        serverFlags &= ~AURA_SERVER_FLAG_NEED_UPDATE;
    }

    void Tick() {}
};