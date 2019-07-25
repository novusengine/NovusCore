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

#include "../../NovusEnums.h"

#include "../../Game/Aura.h"

#define AURALIST_MAX 255
struct AuraListComponent
{
    bool ApplyAura(u64 casterGuid, SpellData spellData, u8 effectIndex)
    {
        i32 availableSlot = GetFreeSlotOrExistingAuraSlot(spellData.Id, effectIndex);
        if (availableSlot == -1)
            return false;

        Aura& aura = auras[availableSlot];
        aura.entityId = entityId;
        aura.unitGuid = unitGuid;
        aura.spellData = spellData;
        aura.effectIndex = effectIndex;
        aura.serverFlags = AURA_SERVER_FLAG_NONE;

        if (spellData.Attributes & 0x40) // 0x40 is the flag for passive auras
            aura.serverFlags |= AURA_SERVER_FLAG_IS_SERVERSIDE;

        aura.slot = availableSlot;
        aura.clientFlags = AURA_CLIENT_FLAG_NONE;

        if (unitGuid == casterGuid)
            aura.clientFlags |= AURA_CLIENT_FLAG_IS_CASTER;

        if (effectIndex == 0)
            aura.clientFlags |= AURA_CLIENT_FLAG_EFFECT_INDEX_1;
        else if (effectIndex == 1)
            aura.clientFlags |= AURA_CLIENT_FLAG_EFFECT_INDEX_2;
        else if (effectIndex == 2)
            aura.clientFlags |= AURA_CLIENT_FLAG_EFFECT_INDEX_3;

        // We need a way to identify if a spell effect is positive or negative.
        aura.clientFlags |= AURA_CLIENT_FLAG_IS_POSITIVE;

        aura.casterLevel = spellData.SpellLevel;
        aura.casterGuid = casterGuid;
        aura.maxDuration = 0; // This should be set to the actual duration ID, and not the duration Index. spellData.DurationIndex;
        aura.duration = 0;

        aura.SetApplied(true);

        if (!aura.IsServerside())
            aura.SetUpdate(true);
        return true;
    }
    bool ApplyAurasFromSpell(u64 casterGuid, SpellData spellData)
    {
        u32 appliedAuras = 0;
        for (i32 i = 0; i < SPELL_EFFECTS_COUNT; i++)
        {
            if (spellData.Effect[i] != SPELL_EFFECT_APPLY_AURA)
                continue;

            if (!ApplyAura(casterGuid, spellData, i))
                continue;

            appliedAuras++;
        }

        return appliedAuras != 0;
    }

    bool RemoveAura(u32 spellId, u8 effectIndex)
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.IsApplied())
                continue;

            if (aura.spellData.Id == spellId && aura.effectIndex == effectIndex)
            {
                aura.SetApplied(false);

                if (!aura.IsServerside())
                    aura.SetUpdate(true);

                return true;
            }
        }

        return false;
    }
    bool RemoveAurasFromSpell(u32 spellId)
    {
        bool unApplied = false;
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.IsApplied())
                continue;

            if (aura.spellData.Id == spellId)
            {
                aura.SetApplied(false);

                if (!aura.IsServerside())
                    aura.SetUpdate(true);

                unApplied = true;
            }
        }

        return unApplied;
    }

    i32 GetFirstFreeAuraSlot()
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.IsApplied())
                return i;
        }

        return -1;
    }
    i32 GetFreeSlotOrExistingAuraSlot(u32 spellId, u8 effectIndex)
    {
        i32 auraIndex = -1;
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.IsApplied())
                continue;

            if (aura.spellData.Id == spellId && aura.effectIndex == effectIndex)
            {
                auraIndex = i;
                break;
            }
        }

        if (auraIndex == -1)
        {
            auraIndex = GetFirstFreeAuraSlot();
        }

        return auraIndex;
    }
    bool HasAura(u32 spellId)
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.IsApplied())
                continue;

            if (aura.spellData.Id == spellId)
                return true;
        }

        return false;
    }
    bool GetAuraBySpellId(u32 spellId, Aura& outAura)
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.IsApplied())
                continue;

            if (aura.spellData.Id == spellId)
            {
                outAura = aura;
                return true;
            }
        }

        return false;
    }

    bool NeedClientUpdate()
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (aura.NeedUpdate())
                return true;
        }

        return false;
    }
    bool CreateFullAuraUpdate(std::shared_ptr<ByteBuffer> buffer)
    {
        u32 writtenAuras = 0;

        buffer->PutGuid(unitGuid);
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            Aura& aura = auras[i];
            if (!aura.NeedUpdate())
                continue;

            aura.CreateAuraUpdate(buffer);
            writtenAuras++;
        }

        return writtenAuras != 0;
    }

    u32 entityId;
    u64 unitGuid;
    Aura auras[AURALIST_MAX];
};