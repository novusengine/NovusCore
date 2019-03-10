/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/

#include "NovusConnection.h"
#include <Networking\ByteBuffer.h>
#include <Networking\Opcode\Opcode.h>
#include <Database\DatabaseConnector.h>

#include <bitset>
#include <zlib.h>

using namespace std::chrono;
static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

#define CLIENT_UPDATE_MASK_BITS 32
template <size_t size>
class UpdateMask
{
public:
    UpdateMask(uint32_t valuesCount)
    {
        Reset();

        _fieldCount = valuesCount;
        _blockCount = (valuesCount + CLIENT_UPDATE_MASK_BITS - 1) / CLIENT_UPDATE_MASK_BITS;
    }

    void SetBit(uint32_t index)
    {
        _bits.set(index);
    }
    void UnsetBit(uint32_t index)
    {
        _bits.reset(index);
    }
    bool IsSet(uint32_t index)
    {
        return _bits.test(index);
    }
    void Reset()
    {
        _bits.reset();
    }

    void AddTo(Common::ByteBuffer& buffer)
    {
        uint32_t maskPart = 0;
        for (uint32_t i = 0; i < GetBlocks() * 32; i++)
        {
            if (IsSet(i))
                maskPart |= 1 << i % 32;

            if ((i + 1) % 32 == 0)
            {
                buffer.Write<uint32_t>(maskPart);
                maskPart = 0;
            }
        }
    }

    uint32_t GetFields() { return _fieldCount; }
    uint32_t GetBlocks() { return _blockCount; }

private:
    uint32_t _fieldCount;
    uint32_t _blockCount;
    std::bitset<size> _bits;
};

class UpdateData
{
public:
    UpdateData() : blockCount(0) { }

    void AddBlock(Common::ByteBuffer const& block)
    {
        data.Append(block);
        ++blockCount;
    }

    bool Build(Common::ByteBuffer& packet)
    {
        Common::ByteBuffer buffer(4 + data._writePos);
        buffer.Write<uint32_t>(blockCount);

        buffer.Append(data);
        size_t pSize = buffer._writePos;

        if (pSize > 100)
        {
            uint32_t destsize = compressBound((uLong)pSize);
            packet.Resize(destsize + sizeof(uint32_t));
            packet._writePos = packet.size();

            packet.Replace<uint32_t>(0, (uint32_t)pSize);
            Compress(const_cast<uint8_t*>(packet.data()) + sizeof(uint32_t), &destsize, (void*)buffer.data(), (int)pSize);
            if (destsize == 0)
                return false;

            packet.Resize(destsize + sizeof(uint32_t));
            packet._writePos = packet.size();
        }
        else
        {
            packet.Append(buffer);
            packet._writePos = packet.size();
            /* This is not hit yet */
        }

        return true;
    }

private:
    uint32_t blockCount;
    Common::ByteBuffer data;

    void Compress(void* dst, uint32_t *dst_size, void* src, int src_size)
    {
        z_stream c_stream;

        c_stream.zalloc = (alloc_func)nullptr;
        c_stream.zfree = (free_func)nullptr;
        c_stream.opaque = (voidpf)nullptr;

        // default Z_BEST_SPEED (1)
        int z_res = deflateInit(&c_stream, 1); // 1 - 9 (9 Being best)
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflateInit) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        c_stream.next_out = (Bytef*)dst;
        c_stream.avail_out = *dst_size;
        c_stream.next_in = (Bytef*)src;
        c_stream.avail_in = (uInt)src_size;

        z_res = deflate(&c_stream, Z_NO_FLUSH);
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflate) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflate) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        if (c_stream.avail_in != 0)
        {
            std::cout << "Can't compress update packet (zlib: deflate not greedy)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflate not greedy)");
            *dst_size = 0;
            return;
        }

        z_res = deflate(&c_stream, Z_FINISH);
        if (z_res != Z_STREAM_END)
        {
            std::cout << "Can't compress update packet (zlib: deflate should report Z_STREAM_END instead %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflate should report Z_STREAM_END instead %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        z_res = deflateEnd(&c_stream);
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflateEnd) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflateEnd) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        *dst_size = c_stream.total_out;
    }
};

/* Setup temporary variables that are neccesary */
static UpdateMask<1344> updateMask(PLAYER_END);
static Common::ByteBuffer playerFields(PLAYER_END * 4);

void SetGuidValue(uint16_t index, uint64_t value)
{
    playerFields.WriteAt<uint64_t>(value, index * 4);
    updateMask.SetBit(index);
    updateMask.SetBit(index + 1);
}
template <typename T>
void SetFieldValue(uint16_t index, T value, uint8_t offset = 0)
{
    playerFields.WriteAt<T>(value, (index * 4) + offset);
    updateMask.SetBit(index);
}

std::unordered_map<uint8_t, NovusMessageHandler> NovusConnection::InitMessageHandlers()
{
    std::unordered_map<uint8_t, NovusMessageHandler> messageHandlers;

    messageHandlers[NOVUS_CHALLENGE]    = { NOVUSSTATUS_CHALLENGE,    sizeof(sNovusChallenge),  &NovusConnection::HandleCommandChallenge };
    messageHandlers[NOVUS_PROOF]        = { NOVUSSTATUS_PROOF,        1,                        &NovusConnection::HandleCommandProof };
    messageHandlers[NOVUS_FOWARDPACKET] = { NOVUSSTATUS_AUTHED,       9,                        &NovusConnection::HandleCommandForwardPacket };

    return messageHandlers;
}
std::unordered_map<uint8_t, NovusMessageHandler> const MessageHandlers = NovusConnection::InitMessageHandlers();

bool NovusConnection::Start()
{
    try
    {
        _socket->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(_address), _port));

        /* NODE_CHALLENGE */
        Common::ByteBuffer packet(6);
        packet.Write<uint8_t>(0);       // Command
        packet.Write<uint8_t>(1);       // Type
        packet.Write<uint16_t>(335);    // Version
        packet.Write<uint16_t>(12340);  // Build

        AsyncRead();
        Send(packet);
        return true;
    }
    catch (asio::system_error error)
    {
        std::cout << "ERROR: " << error.what() << std::endl;
        return false;
    }
}

void NovusConnection::HandleRead()
{
    Common::ByteBuffer& buffer = GetByteBuffer();

    std::cout << "ANTHING AT ALL" << std::endl;
    bool isDecrypted = false;
    while (buffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (!isDecrypted && (_status == NOVUSSTATUS_PROOF || _status == NOVUSSTATUS_AUTHED))
        {
            _crypto->Decrypt(buffer.GetReadPointer(), buffer.GetActualSize());
            isDecrypted = true;
        }

        uint8_t command = buffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            std::cout << "Received HandleRead with no MessageHandler to respond." << std::endl;
            buffer.Clean();
            break;
        }

        if (_status != itr->second.status)
        {
            Close(asio::error::shut_down);
            return;
        }

        if (command == NOVUS_FOWARDPACKET)
        {
            // Check if we should read header
            if (_headerBuffer.GetSpaceLeft() > 0)
            {
                size_t headerSize = std::min(buffer.GetActualSize(), _headerBuffer.GetSpaceLeft());
                _headerBuffer.Write(buffer.GetReadPointer(), headerSize);
                buffer.ReadBytes(headerSize);

                if (_headerBuffer.GetSpaceLeft() > 0)
                {
                    // Wait until we have the entire header
                    assert(buffer.GetActualSize() == 0);
                    break;
                }

                /* Read Header */
                NovusHeader* header = reinterpret_cast<NovusHeader*>(_headerBuffer.GetReadPointer());
                _packetBuffer.Resize(header->size);
                _packetBuffer.ResetPos();
            }

            // We have a header, now check the packet data
            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                std::size_t packetSize = std::min(buffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
                _packetBuffer.Write(buffer.GetReadPointer(), packetSize);
                buffer.ReadBytes(packetSize);

                if (_packetBuffer.GetSpaceLeft() > 0)
                {
                    // Wait until we have all of the packet data
                    assert(buffer.GetActualSize() == 0);
                    break;
                }
            }

            if (!HandleCommandForwardPacket())
            {
                Close(asio::error::shut_down);
                return;
            }
            _headerBuffer.ResetPos();
        }
        else
        {
            uint16_t size = uint16_t(itr->second.packetSize);
            if (buffer.GetActualSize() < size)
                break;

            if (!(*this.*itr->second.handler)())
            {
                Close(asio::error::shut_down);
                return;
            }

            buffer.ReadBytes(size);
        }
    }

    AsyncRead();
}

bool NovusConnection::HandleCommandChallenge()
{
    _status = NOVUSSTATUS_CLOSED;
    sNovusChallenge* novusChallenge = reinterpret_cast<sNovusChallenge*>(GetByteBuffer().GetReadPointer());

    _key->Bin2BN(novusChallenge->K, 32);
    _crypto->SetupClient(_key);

    /* Send fancy encrypted packet here */
    Common::ByteBuffer packet;
    packet.Write<uint8_t>(NOVUS_PROOF); // RELAY_PROOF
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    _status = NOVUSSTATUS_PROOF;

    Send(packet);
    return true;
}
bool NovusConnection::HandleCommandProof()
{
    _status = NOVUSSTATUS_AUTHED;

    return true;
}

bool NovusConnection::HandleCommandForwardPacket()
{
    NovusHeader* header = reinterpret_cast<NovusHeader*>(_headerBuffer.GetReadPointer());
    std::cout << "Received opcode: 0x" << std::hex << std::uppercase << header->opcode << std::endl;

    switch ((Common::Opcode)header->opcode)
    {
        case Common::Opcode::CMSG_PLAYER_LOGIN:
        {
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FOWARDPACKET;
            packetHeader.account = header->account;

            /* This is needed for now as we wouldn't be able to login twice */
            playerFields.Clean();
            playerFields.Resize(PLAYER_END * 4);
            updateMask.Reset();

            uint64_t playerGuid = 0;
            _packetBuffer.Read<uint64_t>(playerGuid);


            /* SMSG_LOGIN_VERIFY_WORLD */
            Common::ByteBuffer verifyWorld;
            packetHeader.opcode = Common::Opcode::SMSG_LOGIN_VERIFY_WORLD;
            packetHeader.size = 4 + (4 * 4);
            packetHeader.AddTo(verifyWorld);

            verifyWorld.Write<uint32_t>(0); // Map (0 == Eastern Kingdom) & Elwynn Forest (Zone is 12) & Northshire (Area is 9)
            verifyWorld.Write<float>(-8949.950195f);
            verifyWorld.Write<float>(-132.492996f);
            verifyWorld.Write<float>(83.531197f);
            verifyWorld.Write<float>(0.0f);
            SendPacket(verifyWorld);


            /* SMSG_ACCOUNT_DATA_TIMES */
            Common::ByteBuffer accountDataForwardPacket;
            Common::ByteBuffer accountDataTimes;
            packetHeader.opcode = Common::Opcode::SMSG_ACCOUNT_DATA_TIMES;

            uint32_t mask = 0xEA;
            accountDataTimes.Write<uint32_t>((uint32_t)time(nullptr));
            accountDataTimes.Write<uint8_t>(1); // bitmask blocks count
            accountDataTimes.Write<uint32_t>(mask); // PER_CHARACTER_CACHE_MASK

            for (uint32_t i = 0; i < 8; ++i)
            {
                if (mask & (1 << i))
                    accountDataTimes.Write<uint32_t>(0);
            }

            packetHeader.size = (uint16_t)accountDataTimes.GetActualSize();
            packetHeader.AddTo(accountDataForwardPacket);
            accountDataForwardPacket.Append(accountDataTimes);
            SendPacket(accountDataForwardPacket);


            /* SMSG_FEATURE_SYSTEM_STATUS */
            Common::ByteBuffer featureSystemStatus;
            packetHeader.opcode = Common::Opcode::SMSG_FEATURE_SYSTEM_STATUS;
            packetHeader.size = 1 + 1;
            packetHeader.AddTo(featureSystemStatus);

            featureSystemStatus.Write<uint8_t>(2);
            featureSystemStatus.Write<uint8_t>(0);
            SendPacket(featureSystemStatus);


            /* SMSG_MOTD */
            Common::ByteBuffer motdForwardPacket;
            Common::ByteBuffer motd;
            packetHeader.opcode = Common::Opcode::SMSG_MOTD;
            packetHeader.AddTo(motd);

            motd.Write<uint32_t>(1);
            motd.WriteString("Welcome");

            packetHeader.size = (uint16_t)motd.GetActualSize();
            packetHeader.AddTo(motdForwardPacket);
            motdForwardPacket.Append(motd);
            SendPacket(motdForwardPacket);


            /* SMSG_LEARNED_DANCE_MOVES */
            Common::ByteBuffer learnedDanceMoves;
            packetHeader.opcode = Common::Opcode::SMSG_LEARNED_DANCE_MOVES;
            packetHeader.size = 4 + 4;
            packetHeader.AddTo(learnedDanceMoves);

            learnedDanceMoves.Write<uint32_t>(0);
            learnedDanceMoves.Write<uint32_t>(0);
            SendPacket(learnedDanceMoves);


            /* SMSG_ACTION_BUTTONS */
            Common::ByteBuffer actionButtons;
            packetHeader.opcode = Common::Opcode::SMSG_ACTION_BUTTONS;
            packetHeader.size = 1 + (4 * 144);
            packetHeader.AddTo(actionButtons);

            actionButtons.Write<uint8_t>(1);
            for (uint8_t button = 0; button < 144; ++button)
            {
                actionButtons.Write<uint32_t>(0);
            }
            SendPacket(actionButtons);


            /* SMSG_LOGIN_SETTIMESPEED */
            Common::ByteBuffer loginSetTimeSpeed;
            packetHeader.opcode = Common::Opcode::SMSG_LOGIN_SETTIMESPEED;
            packetHeader.size = 4 + 4 + 4;
            packetHeader.AddTo(loginSetTimeSpeed);

            tm lt;
            time_t const tmpServerTime = time(nullptr);
            localtime_s(&lt, &tmpServerTime);

            loginSetTimeSpeed.Write<uint32_t>(((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min));
            loginSetTimeSpeed.Write<float>(0.01666667f);
            loginSetTimeSpeed.Write<uint32_t>(0);
            SendPacket(loginSetTimeSpeed);

            /* SMSG_UPDATE_OBJECT */
            SetGuidValue(OBJECT_FIELD_GUID, playerGuid);
            SetFieldValue<uint32_t>(OBJECT_FIELD_TYPE, 0x19); // Object Type Player (Player, Unit, Object)
            SetFieldValue<float>(OBJECT_FIELD_SCALE_X, 1.0f); // Object Type Player (Player, Unit, Object)
            
            SetFieldValue<uint8_t>(UNIT_FIELD_BYTES_0, 1, 0);
            SetFieldValue<uint8_t>(UNIT_FIELD_BYTES_0, 1, 1);
            SetFieldValue<uint8_t>(UNIT_FIELD_BYTES_0, 1, 2);
            SetFieldValue<uint8_t>(UNIT_FIELD_BYTES_0, 1, 3);
            SetFieldValue<uint32_t>(UNIT_FIELD_HEALTH, 60);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER1, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER2, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER3, 100);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER4, 100);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER5, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER6, 8);
            SetFieldValue<uint32_t>(UNIT_FIELD_POWER7, 1000);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXHEALTH, 60);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER1, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER2, 1000);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER3, 100);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER4, 100);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER5, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER6, 8);
            SetFieldValue<uint32_t>(UNIT_FIELD_MAXPOWER7, 1000);
            SetFieldValue<uint32_t>(UNIT_FIELD_LEVEL, 80);
            SetFieldValue<uint32_t>(UNIT_FIELD_FACTIONTEMPLATE, 1);
            SetFieldValue<uint32_t>(UNIT_FIELD_FLAGS, 0x00000008);
            SetFieldValue<uint32_t>(UNIT_FIELD_FLAGS_2, 0x00000800);
            SetFieldValue<uint32_t>(UNIT_FIELD_BASEATTACKTIME + 0, 2900);
            SetFieldValue<uint32_t>(UNIT_FIELD_BASEATTACKTIME + 1, 2000);
            SetFieldValue<uint32_t>(UNIT_FIELD_RANGEDATTACKTIME, 0);
            SetFieldValue<float>(UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
            SetFieldValue<float>(UNIT_FIELD_COMBATREACH, 1.5f);
            SetFieldValue<uint32_t>(UNIT_FIELD_DISPLAYID, 50);
            SetFieldValue<uint32_t>(UNIT_FIELD_NATIVEDISPLAYID, 50);
            SetFieldValue<uint32_t>(UNIT_FIELD_MOUNTDISPLAYID, 0);
            SetFieldValue<float>(UNIT_FIELD_MINDAMAGE, 9.007143f);
            SetFieldValue<float>(UNIT_FIELD_MAXDAMAGE, 11.007143f);
            SetFieldValue<float>(UNIT_FIELD_MINOFFHANDDAMAGE, 0);
            SetFieldValue<float>(UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_BYTES_1, 0);
            SetFieldValue<float>(UNIT_MOD_CAST_SPEED, 1);

            /* 3 individual for loops would make some for nice cache improvements :') */
            for (int i = 0; i < 5; i++)
            {
                SetFieldValue<uint32_t>(UNIT_FIELD_STAT0 + i, 20);
                SetFieldValue<int32_t>(UNIT_FIELD_POSSTAT0 + i, 0);
                SetFieldValue<int32_t>(UNIT_FIELD_NEGSTAT0 + i, 0);
            }

            for (int i = 0; i < 7; i++)
            {
                SetFieldValue<uint32_t>(UNIT_FIELD_RESISTANCES + i, 0);
                SetFieldValue<int32_t>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
                SetFieldValue<int32_t>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
            }
            SetFieldValue<uint32_t>(UNIT_FIELD_STAT0, 42);

            SetFieldValue<uint32_t>(UNIT_FIELD_BASE_MANA, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_BASE_HEALTH, 20);
            SetFieldValue<uint32_t>(UNIT_FIELD_BYTES_2, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_ATTACK_POWER, 29);
            SetFieldValue<uint32_t>(UNIT_FIELD_ATTACK_POWER_MODS, 0);
            SetFieldValue<float>(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
            SetFieldValue<uint32_t>(UNIT_FIELD_RANGED_ATTACK_POWER, 0);
            SetFieldValue<uint32_t>(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
            SetFieldValue<float>(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
            SetFieldValue<float>(UNIT_FIELD_MINRANGEDDAMAGE, 0);
            SetFieldValue<float>(UNIT_FIELD_MAXRANGEDDAMAGE, 0);
            SetFieldValue<float>(UNIT_FIELD_HOVERHEIGHT, 1);

            SetFieldValue<uint32_t>(PLAYER_FLAGS, 0);
            SetFieldValue<uint8_t>(PLAYER_BYTES, 1, 0);
            SetFieldValue<uint8_t>(PLAYER_BYTES, 1, 1);
            SetFieldValue<uint8_t>(PLAYER_BYTES, 1, 2);
            SetFieldValue<uint8_t>(PLAYER_BYTES, 1, 3);
            SetFieldValue<uint8_t>(PLAYER_BYTES_2, 1, 0);
            SetFieldValue<uint8_t>(PLAYER_BYTES_2, 0, 1);
            SetFieldValue<uint8_t>(PLAYER_BYTES_2, 0, 2);
            SetFieldValue<uint8_t>(PLAYER_BYTES_2, 3, 3);
            SetFieldValue<uint8_t>(PLAYER_BYTES_3, 1, 0);
            SetFieldValue<uint8_t>(PLAYER_BYTES_3, 0, 1);
            SetFieldValue<uint8_t>(PLAYER_BYTES_3, 0, 2);
            SetFieldValue<uint8_t>(PLAYER_BYTES_3, 0, 3);

            for (uint8_t slot = 0; slot < 19; ++slot)
            {
                SetGuidValue(PLAYER_FIELD_INV_SLOT_HEAD + (slot * 2), 0);

                SetFieldValue<uint32_t>(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), 0);
                SetFieldValue<uint32_t>(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
            }

            SetFieldValue<uint32_t>(PLAYER_XP, 0);
            SetFieldValue<uint32_t>(PLAYER_NEXT_LEVEL_XP, 400);

            for (int i = 0; i < 127; ++i)
            {
                SetFieldValue<uint32_t>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)), 0);
                SetFieldValue<uint32_t>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, 0);
                SetFieldValue<uint32_t>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 2, 0);
            }

            SetFieldValue<uint32_t>(PLAYER_CHARACTER_POINTS1, 0);
            SetFieldValue<uint32_t>(PLAYER_CHARACTER_POINTS2, 2);
            SetFieldValue<float>(PLAYER_BLOCK_PERCENTAGE, 4.0f);
            SetFieldValue<float>(PLAYER_DODGE_PERCENTAGE, 4.0f);
            SetFieldValue<float>(PLAYER_PARRY_PERCENTAGE, 4.0f);
            SetFieldValue<float>(PLAYER_CRIT_PERCENTAGE, 4.0f);
            SetFieldValue<float>(PLAYER_RANGED_CRIT_PERCENTAGE, 4.0f);
            SetFieldValue<float>(PLAYER_OFFHAND_CRIT_PERCENTAGE, 4.0f);

            for (int i = 0; i < 127; i++)
                SetFieldValue<uint32_t>(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

            SetFieldValue<int32_t>(PLAYER_REST_STATE_EXPERIENCE, 0);
            SetFieldValue<uint32_t>(PLAYER_FIELD_COINAGE, 5000000);

            for (int i = 0; i < 7; i++)
            {
                SetFieldValue<int32_t>(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
                SetFieldValue<int32_t>(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
                SetFieldValue<float>(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
            }

            SetFieldValue<int32_t>(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);
            SetFieldValue<uint32_t>(PLAYER_FIELD_MAX_LEVEL, 80);

            for (int i = 0; i < 3; i++)
            {
                SetFieldValue<float>(PLAYER_RUNE_REGEN_1 + i, 0.1f);
            }

            for (int i = 0; i < 5; i++)
            {
                SetFieldValue<float>(PLAYER_FIELD_GLYPH_SLOTS_1 + i, float(21 + i));
            }

            uint8_t  updateType = 3; // UPDATETYPE_CREATE_OBJECT2
            uint16_t updateFlags = 0x0000; // UPDATEFLAG_NONE

            // Correct & Checked Flags
            updateFlags |= 0x0001; // UPDATEFLAG_SELF
            updateFlags |= 0x0020; // UPDATEFLAG_LIVING
            updateFlags |= 0x0040; // UPDATEFLAG_STATIONARY_POSITION
            
            Common::ByteBuffer buffer(500);
            buffer.Write<uint8_t>(updateType);
            buffer.AppendGuid(playerGuid);
            buffer.Write<uint8_t>(4); // TYPEID_PLAYER

            // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
            buffer.Write<uint16_t>(updateFlags);

            //if (updateFlags & 0x0020) If living
            {
                uint32_t gameTimeMS = uint32_t(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
                //std::cout << "Gametime MS: " << gameTimeMS << std::endl;

                buffer.Write<uint32_t>(0x00); // MovementFlags
                buffer.Write<uint16_t>(0x00); // Extra MovementFlags
                buffer.Write<uint32_t>(gameTimeMS); // Game Time
                // TaggedPosition<Position::XYZO>(pos);
                buffer.Write<float>(-8949.950195f);
                buffer.Write<float>(-132.492996f);
                buffer.Write<float>(83.531197f);
                buffer.Write<float>(0.0f);

                // FallTime
                buffer.Write<uint32_t>(0);

                // Movement Speeds
                buffer.Write<float>(2.5f); // MOVE_WALK
                buffer.Write<float>(7.0f); // MOVE_RUN
                buffer.Write<float>(4.5f); // MOVE_RUN_BACK
                buffer.Write<float>(4.722222f); // MOVE_SWIM
                buffer.Write<float>(2.5f); // MOVE_SWIM_BACK
                buffer.Write<float>(7.0f); // MOVE_FLIGHT
                buffer.Write<float>(4.5f); // MOVE_FLIGHT_BACK
                buffer.Write<float>(3.141593f); // MOVE_TURN_RATE
                buffer.Write<float>(3.141593f); // MOVE_PITCH_RATE
            }

            Common::ByteBuffer fieldBuffer;
            fieldBuffer.Resize(5304);
            UpdateMask<1344> updateMask(PLAYER_END);

            uint16_t _fieldNotifyFlags = UF_FLAG_DYNAMIC;

            uint32_t* flags = UnitUpdateFieldFlags;
            uint32_t visibleFlag = UF_FLAG_PUBLIC;
            visibleFlag |= UF_FLAG_PRIVATE;

            for (uint16_t index = 0; index < PLAYER_END; ++index)
            {
                if (_fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlag) & UF_FLAG_SPECIAL_INFO) 
                    || ((updateType == 0 ? updateMask.IsSet(index) : playerFields.ReadAt<int32_t>(index * 4)) 
                    && (flags[index] & visibleFlag)))
                {
                    updateMask.SetBit(index);

                    if (index == UNIT_NPC_FLAGS)
                    {
                        uint32_t appendValue = playerFields.ReadAt<uint32_t>(UNIT_NPC_FLAGS * 4);

                        /*if (creature)
                            if (!target->CanSeeSpellClickOn(creature))
                                appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;*/

                        fieldBuffer.Write<uint32_t>(appendValue);
                    }
                    else if (index == UNIT_FIELD_AURASTATE)
                    {
                        // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                        uint32_t auraState = playerFields.ReadAt<uint32_t>(UNIT_FIELD_AURASTATE*4) &~(((1 << (14 - 1)) | (1 << (16 - 1))));

                        fieldBuffer.Write<uint32_t>(auraState);
                    }
                    // Seems to be fixed already??
                    // FIXME: Some values at server stored in float format but must be sent to client in uint32 format
                    else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                    {
                        // convert from float to uint32 and send
                        fieldBuffer.Write<uint32_t>(uint32_t(playerFields.ReadAt<int32_t>(index*4)));
                    }
                    // there are some (said float in TC, but all these are ints)int values which may be negative or can't get negative due to other checks
                    else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                        (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                        (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                        (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
                    {
                        fieldBuffer.Write<uint32_t>(uint32_t(playerFields.ReadAt<int32_t>(index * 4)));
                    }
                    // Gamemasters should be always able to select units - remove not selectable flag
                    else if (index == UNIT_FIELD_FLAGS)
                    {
                        uint32_t appendValue = playerFields.ReadAt<uint32_t>(UNIT_FIELD_FLAGS * 4);
                        //if (target->IsGameMaster())
                            //appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                        fieldBuffer.Write<uint32_t>(appendValue);
                    }
                    // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                    else if (index == UNIT_FIELD_DISPLAYID)
                    {
                        uint32_t displayId = playerFields.ReadAt<uint32_t>(UNIT_FIELD_DISPLAYID * 4);
                        /*if (creature)
                        {
                            CreatureTemplate const* cinfo = creature->GetCreatureTemplate();

                            // this also applies for transform auras
                            if (SpellInfo const* transform = sSpellMgr->GetSpellInfo(GetTransformSpell()))
                                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                    if (transform->Effects[i].IsAura(SPELL_AURA_TRANSFORM))
                                        if (CreatureTemplate const* transformInfo = sObjectMgr->GetCreatureTemplate(transform->Effects[i].MiscValue))
                                        {
                                            cinfo = transformInfo;
                                            break;
                                        }

                            if (cinfo->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER)
                                if (target->IsGameMaster())
                                    displayId = cinfo->GetFirstVisibleModel();
                        }*/

                        fieldBuffer.Write<uint32_t>(displayId);
                    }
                    // hide lootable animation for unallowed players
                    else if (index == UNIT_DYNAMIC_FLAGS)
                    {
                        uint32_t dynamicFlags = playerFields.ReadAt<uint32_t>(UNIT_DYNAMIC_FLAGS * 4) & ~(0x4 | 0x08); // UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER

                        /*if (creature)
                        {
                            if (creature->hasLootRecipient())
                            {
                                dynamicFlags |= UNIT_DYNFLAG_TAPPED;
                                if (creature->isTappedBy(target))
                                    dynamicFlags |= UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                            }

                            if (!target->isAllowedToLoot(creature))
                                dynamicFlags &= ~UNIT_DYNFLAG_LOOTABLE;
                        }*/

                        // unit UNIT_DYNFLAG_TRACK_UNIT should only be sent to caster of SPELL_AURA_MOD_STALKED auras
                        //if (dynamicFlags & UNIT_DYNFLAG_TRACK_UNIT)
                            //if (!HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, target->GetGUID()))
                                //dynamicFlags &= ~UNIT_DYNFLAG_TRACK_UNIT;

                        fieldBuffer.Write<uint32_t>(dynamicFlags);
                    }
                    // FG: pretend that OTHER players in own group are friendly ("blue")
                    else if (index == UNIT_FIELD_BYTES_2 || index == UNIT_FIELD_FACTIONTEMPLATE)
                    {
                        //if (IsControlledByPlayer() && target != this && sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && IsInRaidWith(target))
                        //{
                            //FactionTemplateEntry const* ft1 = GetFactionTemplateEntry();
                            //FactionTemplateEntry const* ft2 = target->GetFactionTemplateEntry();
                            //if (!ft1->IsFriendlyTo(*ft2))
                            //{
                                //if (index == UNIT_FIELD_BYTES_2)
                                    // Allow targetting opposite faction in party when enabled in config
                                    //fieldBuffer << (m_uint32Values[UNIT_FIELD_BYTES_2] & ((UNIT_BYTE2_FLAG_SANCTUARY /*| UNIT_BYTE2_FLAG_AURAS | UNIT_BYTE2_FLAG_UNK5*/) << 8)); // this flag is at uint8 offset 1 !!
                                //else
                                    // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                                    //fieldBuffer << uint32(target->GetFaction());
                            //}
                            //else
                                //fieldBuffer << m_uint32Values[index];
                        //}
                        //else
                        fieldBuffer.Write(playerFields.GetDataPointer() + index * 4, 4);
                    }
                    else
                    {
                        // send in current format (float as float, uint32 as uint32)
                        fieldBuffer.Write(playerFields.GetDataPointer() + index * 4, 4);
                    }
                }
            }

            buffer.Write<uint8_t>(updateMask.GetBlocks());
            updateMask.AddTo(buffer);
            buffer.Append(fieldBuffer);

            UpdateData updateData;
            updateData.AddBlock(buffer);

            Common::ByteBuffer tempBuffer;
            updateData.Build(tempBuffer);

            Common::ByteBuffer objectUpdate;
            packetHeader.opcode = Common::Opcode::SMSG_COMPRESSED_UPDATE_OBJECT;
            packetHeader.size = tempBuffer.GetActualSize();
            packetHeader.AddTo(objectUpdate);

            objectUpdate.Append(tempBuffer);
            SendPacket(objectUpdate);


            break;
        }
        case Common::Opcode::CMSG_SET_ACTIVE_VOICE_CHANNEL:
            break;
        case Common::Opcode::CMSG_SET_ACTIVE_MOVER:
        {
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FOWARDPACKET;
            packetHeader.account = header->account;
            packetHeader.opcode = Common::Opcode::SMSG_TIME_SYNC_REQ;
            packetHeader.size = 4;

            Common::ByteBuffer timeSync(9 + 4);
            packetHeader.AddTo(timeSync);

            timeSync.Write<uint32_t>(0);
            SendPacket(timeSync);
            break;
        }
        default:
            std::cout << "Could not handled opcode: 0x" << std::hex << std::uppercase << header->opcode << std::endl;
            break;
    }

    return true;
}

void NovusConnection::SendPacket(Common::ByteBuffer& packet)
{
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    Send(packet);
}