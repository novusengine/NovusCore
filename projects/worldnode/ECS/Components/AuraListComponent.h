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

enum AuraDataFlags
{
    AURADATA_FLAG_NONE = 0x00,
    AURADATA_FLAG_IS_APPLIED = 0x01,
    AURADATA_FLAG_IS_SERVERSIDE = 0x02,
    AURADATA_FLAG_NEED_UPDATE = 0x04
};
struct AuraData
{
    AuraData() : unitGuid(0), spellData(), effectIndex(0), auraDataFlags(AURADATA_FLAG_NONE), slot(0), flags(AURAFLAG_NONE), casterLevel(0), casterGuid(0), maxDuration(0), duration(0) {}

    // Information for the server
    u64 unitGuid;
    SpellData spellData;
    u8 effectIndex;
    u8 auraDataFlags;

    // Information for the client
    u8 slot;
    u8 flags;
    u8 casterLevel;
    u64 casterGuid;
    i32 maxDuration;
    i32 duration;

    bool IsApplied()
    {
        return (auraDataFlags & AURADATA_FLAG_IS_APPLIED);
    }
    bool IsServerside()
    {
        return (auraDataFlags & AURADATA_FLAG_IS_SERVERSIDE);
    }
    bool NeedUpdate()
    {
        return (auraDataFlags & AURADATA_FLAG_NEED_UPDATE);
    }
    void SetApplied(bool state)
    {
        if (state)
        {
            auraDataFlags |= AURADATA_FLAG_IS_APPLIED;

            if (spellData.EffectApplyAuraName[effectIndex] == SPELL_AURA_FLY)
            {
                
            }
        }
        else
        {
            auraDataFlags &= ~AURADATA_FLAG_IS_APPLIED;

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
            auraDataFlags |= AURADATA_FLAG_NEED_UPDATE;
        }
        else
        {
            auraDataFlags &= ~AURADATA_FLAG_NEED_UPDATE;
        }
    }

    bool IsEffect_1()
    {
        return (flags & AURAFLAG_EFFECT_INDEX_1);
    }
    bool IsEffect_2()
    {
        return (flags & AURAFLAG_EFFECT_INDEX_2);
    }
    bool IsEffect_3()
    {
        return (flags & AURAFLAG_EFFECT_INDEX_3);
    }
    bool IsSelfCast()
    {
        return (flags & AURAFLAG_IS_CASTER);
    }
    bool IsPositive()
    {
        return (flags & AURAFLAG_IS_POSITIVE);
    }
    bool HasDuration()
    {
        return (flags & AURAFLAG_HAS_DURATION);
    }
    bool IsNegative()
    {
        return (flags & AURAFLAG_IS_NEGATIVE);
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
            auraBuffer->PutU8(flags);
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

        auraDataFlags &= ~AURADATA_FLAG_NEED_UPDATE;
    }

    void Tick() {}
};

#define AURALIST_MAX 255
struct AuraListComponent
{
    bool ApplyAura(u64 casterGuid, SpellData spellData, u8 effectIndex)
    {
        i32 availableSlot = GetFreeSlotOrExistingAuraSlot(spellData.Id, effectIndex);
        if (availableSlot == -1)
            return false;

        AuraData& auraData = auras[availableSlot];
        auraData.unitGuid = unitGuid;
        auraData.spellData = spellData;
        auraData.effectIndex = effectIndex;
        auraData.auraDataFlags = AURADATA_FLAG_NONE;

        if (spellData.Attributes & 0x40) // 0x40 is the flag for passive auras
            auraData.auraDataFlags |= AURADATA_FLAG_IS_SERVERSIDE;

        auraData.slot = availableSlot;
        auraData.flags = AURAFLAG_NONE;

        if (unitGuid == casterGuid)
            auraData.flags |= AURAFLAG_IS_CASTER;

        if (effectIndex == 0)
            auraData.flags |= AURAFLAG_EFFECT_INDEX_1;
        else if (effectIndex == 1)
            auraData.flags |= AURAFLAG_EFFECT_INDEX_2;
        else if (effectIndex == 2)
            auraData.flags |= AURAFLAG_EFFECT_INDEX_3;

        // We need a way to identify if a spell effect is positive or negative.
        auraData.flags |= AURAFLAG_IS_POSITIVE;

        auraData.casterLevel = spellData.SpellLevel;
        auraData.casterGuid = casterGuid;
        auraData.maxDuration = 0; // This should be set to the actual duration ID, and not the duration Index. spellData.DurationIndex;
        auraData.duration = 0;

        auraData.SetApplied(true);

        if (!auraData.IsServerside())
            auraData.SetUpdate(true);
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
            AuraData& auraData = auras[i];
            if (!auraData.IsApplied())
                continue;

            if (auraData.spellData.Id == spellId && auraData.effectIndex == effectIndex)
            {
                auraData.SetApplied(false);

                if (!auraData.IsServerside())
                    auraData.SetUpdate(true);

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
            AuraData& auraData = auras[i];
            if (!auraData.IsApplied())
                continue;

            if (auraData.spellData.Id == spellId)
            {
                auraData.SetApplied(false);

                if (!auraData.IsServerside())
                    auraData.SetUpdate(true);

                unApplied = true;
            }
        }

        return unApplied;
    }

    i32 GetFirstFreeAuraSlot()
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            AuraData& auraData = auras[i];
            if (!auraData.IsApplied())
                return i;
        }

        return -1;
    }
    i32 GetFreeSlotOrExistingAuraSlot(u32 spellId, u8 effectIndex)
    {
        i32 auraIndex = -1;
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            AuraData& auraData = auras[i];
            if (!auraData.IsApplied())
                continue;

            if (auraData.spellData.Id == spellId && auraData.effectIndex == effectIndex)
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
            AuraData& auraData = auras[i];
            if (!auraData.IsApplied())
                continue;

            if (auraData.spellData.Id == spellId)
                return true;
        }

        return false;
    }
    bool GetAuraBySpellId(u32 spellId, AuraData& outAuraData)
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            AuraData& auraData = auras[i];
            if (!auraData.IsApplied())
                continue;

            if (auraData.spellData.Id == spellId)
            {
                outAuraData = auraData;
                return true;
            }
        }

        return false;
    }

    bool NeedClientUpdate()
    {
        for (i32 i = 0; i < AURALIST_MAX; i++)
        {
            AuraData& auraData = auras[i];
            if (auraData.NeedUpdate())
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
            AuraData& auraData = auras[i];
            if (!auraData.NeedUpdate())
                continue;

            auraData.CreateAuraUpdate(buffer);
            writtenAuras++;
        }

        return writtenAuras != 0;
    }

    u64 unitGuid;
    AuraData auras[AURALIST_MAX];
};